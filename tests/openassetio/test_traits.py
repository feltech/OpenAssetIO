"""
Tests for the traits type system.

TODO(DF): Notes:
    * Do we need _required_ properties, e.g. BlobTrait is wrong without URL?
    * Test trait accessors to SpecificationData where the property exists but is an unexpected type
      in the variant.
"""
# pylint: disable=invalid-name,missing-class-docstring,missing-function-docstring
# pylint: disable=redefined-outer-name,no-self-use
import pytest

from openassetio import specification

"""
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
"""

class Test_SpecificationData:
    def test_valid_values(self, a_specification_data): #, a_simple_map):
        a_specification_data.setTraitProperty("a trait id", "a string", "string")
        a_specification_data.setTraitProperty("a trait id", "an int", 1)
        a_specification_data.setTraitProperty("a trait id", "a float", 1.0)
        a_specification_data.setTraitProperty("a trait id", "a bool", True)
        # a_specification_data.setTraitProperty("a trait id", "a map", a_simple_map)

        assert a_specification_data.getTraitProperty("a trait id", "a string") == "string"
        assert isinstance(a_specification_data.getTraitProperty("a trait id", "a string"), str)
        assert a_specification_data.getTraitProperty("a trait id", "an int") == 1
        assert isinstance(a_specification_data.getTraitProperty("a trait id", "an int"), int)
        assert a_specification_data.getTraitProperty("a trait id", "a float") == 1.0
        assert isinstance(a_specification_data.getTraitProperty("a trait id", "a float"), float)
        assert a_specification_data.getTraitProperty("a trait id", "a bool") is True
        # assert a_specification_data.getTraitProperty("a trait id", "a map") is a_simple_map

    def test_when_key_is_not_found_then_returns_None(self, a_specification_data):
        assert a_specification_data.getTraitProperty("a trait id", "a string") is None
        # Set a value under the same trait but different key.
        a_specification_data.setTraitProperty("a trait id", "an int", 1)
        assert a_specification_data.getTraitProperty("a trait id", "a string") is None

    def test_when_key_is_not_str_then_TypeError_raised(self, a_specification_data):
        with pytest.raises(TypeError):
            _ = a_specification_data.getTraitProperty(123, "a string")

        with pytest.raises(TypeError):
            a_specification_data.setTraitProperty(123, "a string", "string")

        with pytest.raises(TypeError):
            a_specification_data.setTraitProperty("a trait id", 123, "string")

    def test_when_value_is_not_supported_then_TypeError_raised(self, a_specification_data):
        with pytest.raises(TypeError):
            a_specification_data.setTraitProperty("a trait id", "unknown type", object())


"""
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
"""

"""
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
"""

@pytest.fixture
def a_specification_data():
    return specification.SpecificationData()

"""
@pytest.fixture
def a_simple_map():
    return traits.SimpleMap()
"""
