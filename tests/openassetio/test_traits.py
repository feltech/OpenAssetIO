"""
Tests for the traits type system.

TODO(DF): Notes:
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

        for trait_id in entity_spec.kTraitIDs:
            if trait_id == BlobTrait.kID:
                blob_trait = entity_spec.getTrait(BlobTrait.kID)
                url = blob_trait.getUrl()
                mime_type = blob_trait.getMimeType()
            elif trait_id == ImageTrait.kID:
                is_image = True

        if url is not None:
            if mime_type is None:
                new_ref = my_ams.manage_arbitrary_file(url, is_image)
            else:
                new_ref = my_ams.manage_file_of_type(url, mime_type, is_image)
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
                if mime_type is None:
                    new_ref = my_ams.manage_arbitrary_file(url, is_image)
                else:
                    new_ref = my_ams.manage_file_of_type(url, mime_type, is_image)
                responses[idx] = new_ref
```
or
```
def register(self, entityRefs, entitySpecs, context, hostSession):
    responses = [RegisterError(f"Failed to register {entity_ref}") for entity_ref in entityRefs]
    for idx, entity_ref, entity_spec in enumerate(zip(entityRefs, entitySpecs)):

        # hasTraitData() to ensure not just has trait but trait has properties.
        should_manage = entity_spec.hasTraitData(BlobTrait.kID)

        if should_manage:
            is_image = entity_spec.hasTrait(ImageTrait.kID)
            blob_trait = entity_spec.getTrait(BlobTrait.kID)
            url = blob_trait.getUrl()
            mime_type = blob_trait.getMimeType()
            if url is not None:
                if mime_type is None:
                    new_ref = my_ams.manage_arbitrary_file(url, is_image)
                else:
                    new_ref = my_ams.manage_file_of_type(url, mime_type, is_image)
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
                trait_list_str =  ",".join(entity_spec.kTraitIDs)
                if mime_type is None:
                    new_ref = my_ams.manage_arbitrary_file(url, trait_list_str)
                else:
                    new_ref = my_ams.manage_file_of_type(url, mime_type, trait_list_str)
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


def test_BlobSpecification_data(a_blob_specification):
    data = a_blob_specification.data()

    assert isinstance(data, traits.SpecificationData)
    assert len(data) == 1  # Initialised with (empty) BlobTrait.

    props = data[traits.BlobTrait.kName]
    assert isinstance(props, traits.Properties)
    assert len(props) == 0


# TODO(DF): Should these tests be repeated for every property of every trait, or one sample
#  property per trait (as below), or is there some way we can test this through the base class?
class Test_BlobSpecification_blobTrait:
    def test_when_spec_data_changed_then_trait_is_updated(
            self, a_blob_specification, a_blob_specifications_data):
        blobTrait = a_blob_specification.blobTrait()
        expected = "updated://url"
        a_blob_specifications_data[traits.BlobTrait.kName]["url"] = expected

        actual = blobTrait.getURL()

        assert actual == expected

    def test_when_trait_data_changed_then_spec_data_is_updated(
            self, a_blob_specification, a_blob_specifications_data):
        blobTrait = a_blob_specification.blobTrait()
        expected = "updated://url"

        blobTrait.setURL(expected)

        actual = a_blob_specifications_data[traits.BlobTrait.kName]["url"]
        assert actual == expected


class Test_SpecificationDict:
    def test_valid_values(self, a_specification_data, a_trait_properties):
        a_specification_data["a trait name"] = a_trait_properties

        assert a_specification_data["a trait name"] is a_trait_properties


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


def test_BlobTrait_kName():
    assert traits.BlobTrait.kName == "blob"


class Test_BlobTrait_getURL:
    def test_when_data_has_no_url_then_returns_None(self, a_specification_data):
        assert traits.BlobTrait(a_specification_data).getURL() is None

    def test_when_data_has_url_then_returns_url(self, a_blob_specifications_data):
        expected = "some://url"
        a_blob_specifications_data[traits.BlobTrait.kName]["url"] = expected

        actual = traits.BlobTrait(a_blob_specifications_data).getURL()

        assert actual == expected


class Test_BlobTrait_setURL:
    def test_when_url_is_a_str_then_url_is_added_to_dict(self, a_specification_data):
        expected = "some://url"

        traits.BlobTrait(a_specification_data).setURL(expected)

        assert a_specification_data[traits.BlobTrait.kName]["url"] == expected

    def test_when_url_is_None_then_url_is_removed_from_dict(self, a_blob_specifications_data):
        a_blob_specifications_data[traits.BlobTrait.kName]["url"] = "some://url"

        traits.BlobTrait(a_blob_specifications_data).setURL(None)

        assert "url" not in a_blob_specifications_data[traits.BlobTrait.kName]

    def test_when_url_is_wrong_type_then_TypeError_is_raised(self, a_blob_specifications_data):
        with pytest.raises(TypeError):
            traits.BlobTrait(a_blob_specifications_data).setURL(123)


class Test_BlobTrait_getMimeType:
    def test_when_data_has_no_mimeType_then_returns_None(self, a_specification_data):
        assert traits.BlobTrait(a_specification_data).getMimeType() is None

    def test_when_data_has_mimeType_then_returns_mimeType(self, a_blob_specifications_data):
        expected = "some://url"
        a_blob_specifications_data[traits.BlobTrait.kName]["mimeType"] = expected

        actual = traits.BlobTrait(a_blob_specifications_data).getMimeType()

        assert actual == expected


class Test_BlobTrait_setMimeType:
    def test_when_mimeType_is_a_str_then_mimeType_is_added_to_dict(
            self, a_blob_specifications_data):
        expected = "application/x-something"

        traits.BlobTrait(a_blob_specifications_data).setMimeType(expected)

        assert a_blob_specifications_data[traits.BlobTrait.kName]["mimeType"] == expected

    def test_when_mimeType_is_None_then_mimeType_is_removed_from_dict(
            self, a_blob_specifications_data):
        a_blob_specifications_data[traits.BlobTrait.kName]["mimeType"] = "application/x-something"

        traits.BlobTrait(a_blob_specifications_data).setMimeType(None)

        assert "mimeType" not in a_blob_specifications_data[traits.BlobTrait.kName]

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
