import types

import pytest

from openassetio import traits

"""
TODO(DF): Questions:
    Notes:
    * The dict-like containers _can_ work out of the box with pybind11 auto-converting, but this
      requires copies whenever we go from C++<->Python. Alternatively, STL containers can be bound
      as opaque, see https://pybind11.readthedocs.io/en/stable/advanced/cast/stl.html#binding-stl-containers
    
"""


class Test_SpecificationDict():
    def test_valid_values(self, a_specification_data, a_trait_properties):
        a_specification_data["a trait name"] = a_trait_properties

        assert a_specification_data["a trait name"] is a_trait_properties

    def test_when_key_is_not_found_then_value_is_None(self, a_specification_data):
        assert a_specification_data["missing key"] is None

    def test_when_key_is_not_str_then_TypeError_raised(
            self, a_specification_data, a_trait_properties):
        with pytest.raises(TypeError):
            _ = a_specification_data[123]

        with pytest.raises(TypeError):
            a_specification_data[123] = a_trait_properties

    def test_when_value_is_not_supported_then_TypeError_raised(self, a_specification_data):
        with pytest.raises(TypeError):
            a_specification_data["key"] = object()


class Test_Properties():
    def test_valid_values(self, a_trait_properties, a_simple_map):
        a_trait_properties["a string"] = "string"
        a_trait_properties["an int"] = 1
        a_trait_properties["a float"] = 1.0
        a_trait_properties["a bool"] = True
        a_trait_properties["a map"] = a_simple_map
        a_trait_properties["a default"] = None

        assert a_trait_properties["a string"] == "string"
        assert isinstance(a_trait_properties["a string"], str)
        assert a_trait_properties["an int"] == 1
        assert isinstance(a_trait_properties["an int"], int)
        assert a_trait_properties["a float"] == 1.0
        assert isinstance(a_trait_properties["a float"], float)
        assert a_trait_properties["a bool"] is True
        assert a_trait_properties["a map"] is a_simple_map
        assert a_trait_properties["a default"] is None

    def test_when_key_is_not_found_then_value_is_None(self, a_trait_properties):
        assert a_trait_properties["missing key"] is None

    def test_when_key_is_not_str_then_TypeError_raised(self, a_trait_properties):
        with pytest.raises(TypeError):
            _ = a_trait_properties[123]

        with pytest.raises(TypeError):
            a_trait_properties[123] = 1

    def test_when_value_is_not_supported_then_TypeError_raised(self, a_trait_properties):
        with pytest.raises(TypeError):
            a_trait_properties["key"] = object()


class Test_SimpleMap():
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

    def test_when_key_is_not_found_then_KeyError_is_raised(self, a_trait_properties):
        with pytest.raises(KeyError):
            _ = a_trait_properties["missing key"]

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


def test_BlobTrait_kName():
    assert traits.BlobTrait.kName == "blob"


class Test_BlobTrait_getURL:
    def test_when_data_has_no_url_then_returns_None(self, a_specification_data):
        assert traits.BlobTrait(a_specification_data).getURL() is None

    def test_when_data_has_url_then_returns_url(self, a_specification_data):
        expected = "some://url"
        a_specification_data[traits.BlobTrait.kName]["url"] = expected

        actual = traits.BlobTrait(a_specification_data).getURL()

        assert actual == expected


class Test_BlobTrait_setURL:
    def test_when_url_is_a_str_then_url_is_added_to_dict(self, a_specification_data):
        expected = "some://url"

        traits.BlobTrait(a_specification_data).setURL(expected)

        assert a_specification_data[traits.BlobTrait.kName]["url"] == expected

    def test_when_url_is_None_then_url_is_unset_in_dict(self, a_specification_data):
        a_specification_data[traits.BlobTrait.kName]["url"] = "some://url"

        traits.BlobTrait(a_specification_data).setURL(None)

        assert a_specification_data[traits.BlobTrait.kName]["url"] is None

    def test_when_url_is_wrong_type_then_TypeError_is_raised(self, a_specification_data):
        with pytest.raises(TypeError):
            traits.BlobTrait(a_specification_data).setURL(123)


class Test_BlobTrait_getMimeType:
    def test_when_data_has_no_mimeType_then_returns_None(self, a_specification_data):
        assert traits.BlobTrait(a_specification_data).getMimeType() is None

    def test_when_data_has_mimeType_then_returns_mimeType(self, a_specification_data):
        expected = "some://url"
        a_specification_data[traits.BlobTrait.kName]["mimeType"] = expected

        actual = traits.BlobTrait(a_specification_data).getMimeType()

        assert actual == expected


class Test_BlobTrait_setMimeType:
    def test_when_mimeType_is_a_str_then_mimeType_is_added_to_dict(self, a_specification_data):
        expected = "application/x-something"

        traits.BlobTrait(a_specification_data).setMimeType(expected)

        assert a_specification_data[traits.BlobTrait.kName]["mimeType"] == expected

    def test_when_mimeType_is_None_then_mimeType_is_unset_in_dict(self, a_specification_data):
        a_specification_data[traits.BlobTrait.kName]["mimeType"] = "application/x-something"

        traits.BlobTrait(a_specification_data).setMimeType(None)

        assert a_specification_data[traits.BlobTrait.kName]["mimeType"] is None

    def test_when_mimeType_is_wrong_type_then_TypeError_is_raised(self, a_specification_data):
        with pytest.raises(TypeError):
            traits.BlobTrait(a_specification_data).setMimeType(123)


@pytest.fixture
def a_specification_data():
    return traits.SpecificationData()


@pytest.fixture
def a_trait_properties():
    return traits.Properties()


@pytest.fixture
def a_simple_map():
    return traits.SimpleMap()
