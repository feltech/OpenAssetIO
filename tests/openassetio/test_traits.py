"""
Tests for the traits type system.

TODO(DF): Notes:
    * Do we need _required_ properties, e.g. BlobTrait is wrong without URL?
    * The dict-like containers _can_ work out of the box with pybind11 auto-converting, but this
      requires copies whenever we go from C++<->Python. Alternatively, STL containers can be bound
      as opaque (via some handy util functions), which is the approach assumed below.
      See https://pybind11.readthedocs.io/en/stable/advanced/cast/stl.html#binding-stl-containers


# Example: resolve then register

Spec:
```
class ImageSpecification(BaseSpecification):
  kTraitIDs = (BlobTrait.kID, ImageTrait.kID)
```

Host:
```
# Resolve an image

[data] = manager.resolve([initial_ref], ImageSpecification.kTraitIDs, context)

# Extract metadata about the image.

initial_spec = ImageSpecification(data)
path = img.blobTrait().getUrl()
mime_type = img.blobTrait().getUrl()  # == None
color_space = img.imageTrait().getColorSpace()  # == None

# Do some processing on the image

new_path = update_image(path, mime_type, color_space)

new_spec = ImageSpecification()
new_spec.blobTrait().setUrl(new_path)
new_spec.blobTrait().setMimeType(get_mime_type(new_path))

[new_ref] = manager.register([initial_ref], [new_spec], context)
```

ManagerInterface:
```
def resolve(self, entityRefs, traitIDs, context, hostSession):
  responses = []
  for entityRef in entityRefs:
    response = SpecificationData()
    responses.append(response)
    # Alternatively: `if BlobTrait.kID in traitIDs`
    for trait_id in traitIDs:
      if trait_id == BlobTrait.kID:
        BlobTrait(response).setURL(my_ams.get_url_for_ref(entityRef))

  return responses

def register(self, entityRefs, entitySpecs, context, hostSession):
    responses = [RegisterError(f"Failed to register {entity_ref}") for entity_ref in entityRefs]
    for idx, entity_ref, entity_spec in enumerate(zip(entityRefs, entitySpecs)):
        is_image = False
        url = None
        mime_type = None

        for trait_id in entity_spec.traitIDs():
            if trait_id == BlobTrait.kID:
                blob_trait = entity_spec.getTrait(BlobTrait.kID)
                url = blob_trait.getUrl()
                mime_type = blob_trait.getMimeType()
            elif trait_id == ImageTrait.kID:
                is_image = True

        if url is not None:
            new_ref = my_ams.manage_file(url, mime_type, is_image)
            responses[idx] = new_ref
```
or
```
def register(self, entityRefs, entitySpecs, context, hostSession):
    responses = [RegisterError(f"Failed to register {entity_ref}") for entity_ref in entityRefs]
    for idx, entity_ref, entity_spec in enumerate(zip(entityRefs, entitySpecs)):
        is_image = False
        url = None
        mime_type = None
        blob_trait = entity_spec.getTrait(BlobTrait.kID)

        if blob_trait is not None:
            url = blob_trait.getUrl()
            if url is not None:
                image_trait = entity_spec.getTrait(ImageTrait.kID)
                is_image = image_trait is not None
                mime_type = blob_trait.getMimeType()
                new_ref = my_ams.manage_file(url, mime_type, is_image)
                responses[idx] = new_ref
```
or
```
class BlobTrait:
    kID = "blob"
    kExpectedProps = ["url"]


def register(self, entityRefs, entitySpecs, context, hostSession):
    responses = [RegisterError(f"Failed to register {entity_ref}") for entity_ref in entityRefs]
    for idx, entity_ref, entity_spec in enumerate(zip(entityRefs, entitySpecs)):

        # hasTraitData() to ensure not just has trait but trait has properties.
        should_manage = entity_spec.hasTraitAndExpectedProps(BlobTrait.kID)

        if should_manage:
            is_image = entity_spec.hasTrait(ImageTrait.kID)
            blob_trait = entity_spec.getTrait(BlobTrait.kID)
            url = blob_trait.getUrl()
            mime_type = blob_trait.getMimeType()
            if url is not None:
                new_ref = my_ams.manage_file(url, mime_type, is_image)
                responses[idx] = new_ref
```
or
```
def register(self, entityRefs, entitySpecs, context, hostSession):
    responses = [RegisterError(f"Failed to register {entity_ref}") for entity_ref in entityRefs]
    for idx, entity_ref, entity_spec in enumerate(zip(entityRefs, entitySpecs)):

        blob_trait = BlobTrait(entity_spec)

        if blob_trait.isValid():
            is_image = ImageTrait(entity_spec).isValid()
            url = blob_trait.getUrl()
            mime_type = blob_trait.getMimeType()
            if url is not None:
                new_ref = my_ams.manage_file(url, mime_type, is_image)
                responses[idx] = new_ref
```
"""
# pylint: disable=invalid-name,missing-class-docstring,missing-function-docstring
# pylint: disable=redefined-outer-name,no-self-use
# TODO(DF): @pylint - re-enable once implementation exists
# pylint: disable=no-member
import pytest

from openassetio import traits


def test_BaseSpecification_data():
    data = traits.BaseSpecification().data()

    assert isinstance(data, traits.SpecificationData)
    assert len(data) == 0


def test_BaseSpecification_traitIDs():
    assert traits.BaseSpecification().traitIDs() == tuple()


def test_BlobSpecification_traitIDs():
    assert traits.BlobSpecification().traitIDs() == (traits.BlobTrait.kID,)


class Test_BlobSpecification_blobTrait:
    def test(self, a_blob_specification):
        blob_trait = a_blob_specification.blobTrait()

        assert isinstance(blob_trait, traits.BlobTrait)
        assert blob_trait.isValid()
        assert isinstance(blob_trait.data(), traits.SpecificationData)
        assert blob_trait.data() is a_blob_specification.data()


class Test_SpecificationData:
    def test_valid_values(self, a_specification_data, a_trait_properties):
        a_specification_data["a trait name"] = a_trait_properties

        assert a_specification_data["a trait name"] is a_trait_properties

    def test_when_key_is_not_found_then_raises_KeyError(self, a_specification_data):
        with pytest.raises(KeyError):
            _ = a_specification_data["missing key"]

        with pytest.raises(KeyError):
            a_specification_data["missing key"] = "_"

    def test_when_key_is_not_str_then_TypeError_raised(
            self, a_specification_data, a_trait_properties):
        with pytest.raises(TypeError):
            _ = a_specification_data[123]

        with pytest.raises(TypeError):
            a_specification_data[123] = a_trait_properties

    def test_when_value_is_not_supported_then_TypeError_raised(self, a_specification_data):
        with pytest.raises(TypeError):
            a_specification_data["key"] = object()


class Test_Properties:
    def test_valid_values(self, a_trait_properties, a_simple_map):
        a_trait_properties["a string"] = "string"
        a_trait_properties["an int"] = 1
        a_trait_properties["a float"] = 1.0
        a_trait_properties["a bool"] = True
        a_trait_properties["a map"] = a_simple_map

        assert a_trait_properties["a string"] == "string"
        assert isinstance(a_trait_properties["a string"], str)
        assert a_trait_properties["an int"] == 1
        assert isinstance(a_trait_properties["an int"], int)
        assert a_trait_properties["a float"] == 1.0
        assert isinstance(a_trait_properties["a float"], float)
        assert a_trait_properties["a bool"] is True
        assert a_trait_properties["a map"] is a_simple_map

    def test_when_key_is_not_found_then_raises_KeyError(self, a_trait_properties):
        with pytest.raises(KeyError):
            _ = a_trait_properties["missing key"]

        with pytest.raises(KeyError):
            a_trait_properties["missing key"] = "_"

    def test_when_key_is_not_str_then_TypeError_raised(self, a_trait_properties):
        with pytest.raises(TypeError):
            _ = a_trait_properties[123]

        with pytest.raises(TypeError):
            a_trait_properties[123] = 1

    def test_when_value_is_not_supported_then_TypeError_raised(self, a_trait_properties):
        with pytest.raises(TypeError):
            a_trait_properties["key"] = object()


class Test_SimpleMap:
    def test_valid_values(self, a_simple_map):
        a_simple_map["a string"] = "string"
        a_simple_map["an int"] = 1
        a_simple_map["a float"] = 1.0
        a_simple_map["a bool"] = True

        assert a_simple_map["a string"] == "string"
        assert isinstance(a_simple_map["a string"], str)
        assert a_simple_map["an int"] == 1
        assert isinstance(a_simple_map["an int"], int)
        assert a_simple_map["a float"] == 1.0
        assert isinstance(a_simple_map["a float"], float)
        assert a_simple_map["a bool"] is True

    def test_when_key_is_not_found_then_KeyError_is_raised(self, a_simple_map):
        with pytest.raises(KeyError):
            _ = a_simple_map["missing key"]

        with pytest.raises(KeyError):
            a_simple_map["missing key"] = 1

    def test_when_key_is_not_str_then_TypeError_raised(self, a_simple_map):
        with pytest.raises(TypeError):
            _ = a_simple_map[123]

        with pytest.raises(TypeError):
            a_simple_map[123] = 1

    def test_when_value_is_not_primitive_then_TypeError_raised(self, a_simple_map):
        with pytest.raises(TypeError):
            a_simple_map["key"] = None

        with pytest.raises(TypeError):
            a_simple_map["key"] = object()


def test_BlobTrait_kID():
    assert traits.BlobTrait.kID == "blob"


class Test_BlobTrait_isValid:
    def test_when_wrapping_blob_spec_then_returns_true(self, a_blob_specification):
        assert traits.BlobTrait(a_blob_specification).isValid()

    def test_when_wrapping_arbitary_data_then_returns_true(self):
        assert traits.BlobTrait(traits.SpecificationData()).isValid()

    def test_when_wrapping_unsupported_spec_then_returns_false(self):
        assert not traits.BlobTrait(traits.BaseSpecification()).isValid()


class Test_BlobTrait_data:
    def test_when_wrapping_blob_spec_then_returns_specs_data(self, a_blob_specification):
        assert traits.BlobTrait(a_blob_specification).data() is a_blob_specification.data()

    def test_when_wrapping_arbitrary_data_then_returns_data(self):
        data = traits.SpecificationData()
        assert traits.BlobTrait(data).data() is data

    def test_when_wrapping_unsupported_spec_then_returns_None(self):
        spec = traits.BaseSpecification()
        assert traits.BlobTrait(spec).data() is None


class Test_BlobTrait_getURL:
    def test_when_data_has_no_url_then_returns_None(self, a_specification_data):
        assert traits.BlobTrait(a_specification_data).getURL() is None

    def test_when_data_has_url_then_returns_url(self, a_blob_specifications_data):
        expected = "some://url"
        a_blob_specifications_data[traits.BlobTrait.kID]["url"] = expected

        actual = traits.BlobTrait(a_blob_specifications_data).getURL()

        assert actual == expected


class Test_BlobTrait_setURL:
    def test_when_url_is_a_str_then_url_is_added_to_dict(self, a_specification_data):
        expected = "some://url"

        traits.BlobTrait(a_specification_data).setURL(expected)

        assert a_specification_data[traits.BlobTrait.kID]["url"] == expected

    def test_when_url_is_None_then_url_is_removed_from_dict(self, a_blob_specifications_data):
        a_blob_specifications_data[traits.BlobTrait.kID]["url"] = "some://url"

        traits.BlobTrait(a_blob_specifications_data).setURL(None)

        assert "url" not in a_blob_specifications_data[traits.BlobTrait.kID]

    def test_when_url_is_wrong_type_then_TypeError_is_raised(self, a_blob_specifications_data):
        with pytest.raises(TypeError):
            traits.BlobTrait(a_blob_specifications_data).setURL(123)


class Test_BlobTrait_getMimeType:
    def test_when_data_has_no_mimeType_then_returns_None(self, a_specification_data):
        assert traits.BlobTrait(a_specification_data).getMimeType() is None

    def test_when_data_has_mimeType_then_returns_mimeType(self, a_blob_specifications_data):
        expected = "some://url"
        a_blob_specifications_data[traits.BlobTrait.kID]["mimeType"] = expected

        actual = traits.BlobTrait(a_blob_specifications_data).getMimeType()

        assert actual == expected


class Test_BlobTrait_setMimeType:
    def test_when_mimeType_is_a_str_then_mimeType_is_added_to_dict(
            self, a_blob_specifications_data):
        expected = "application/x-something"

        traits.BlobTrait(a_blob_specifications_data).setMimeType(expected)

        assert a_blob_specifications_data[traits.BlobTrait.kID]["mimeType"] == expected

    def test_when_mimeType_is_None_then_mimeType_is_removed_from_dict(
            self, a_blob_specifications_data):
        a_blob_specifications_data[traits.BlobTrait.kID]["mimeType"] = "application/x-something"

        traits.BlobTrait(a_blob_specifications_data).setMimeType(None)

        assert "mimeType" not in a_blob_specifications_data[traits.BlobTrait.kID]

    def test_when_mimeType_is_wrong_type_then_TypeError_is_raised(
            self,
            a_blob_specifications_data):
        with pytest.raises(TypeError):
            traits.BlobTrait(a_blob_specifications_data).setMimeType(123)


@pytest.fixture
def a_blob_specifications_data(a_blob_specification):
    return a_blob_specification.data()


@pytest.fixture
def a_blob_specification():
    return traits.BlobSpecification()


@pytest.fixture
def a_specification_data():
    return traits.SpecificationData()


@pytest.fixture
def a_trait_properties():
    return traits.Properties()


@pytest.fixture
def a_simple_map():
    return traits.SimpleMap()
