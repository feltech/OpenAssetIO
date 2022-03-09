"""
Tests for the traits type system.

TODO(DF): Notes:
    * Do we need _required_ properties, e.g. BlobTrait is wrong without URL?
    * Test trait accessors to SpecificationData where the property exists but is an unexpected type
      in the variant.
    * Add SpecificationBase / TraitBase for Python-defined specifications and/or test that abstract
      bases cannot be instantiated.
"""
# pylint: disable=invalid-name,missing-class-docstring,missing-function-docstring
# pylint: disable=redefined-outer-name,no-self-use
import pytest

from openassetio import specification
from openassetio.specification import trait


def test_BlobSpecification_traitIDs():
    assert specification.BlobSpecification().traitIDs() == [trait.BlobTrait.traitId()]


class Test_SpecificationData:
    def test_valid_values(self, a_specification_data):  # , a_simple_map):
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


def test_BlobTrait_traitId():
    assert trait.BlobTrait.traitId() == "blob"


class Test_BlobTrait_isValid:
    def test_when_wrapping_blob_spec_then_returns_true(self, a_blob_specification):
        assert trait.BlobTrait(a_blob_specification).isValid()

    def test_when_wrapping_arbitary_data_then_returns_true(self):
        assert trait.BlobTrait(specification.SpecificationData()).isValid()

    # TODO(DF): fix this
    # def test_when_wrapping_unsupported_spec_then_returns_false(self):
    #     assert not trait.BlobTrait(trait.BaseSpecification()).isValid()


class Test_BlobTrait_data:
    def test_when_wrapping_blob_spec_then_returns_specs_data(self, a_blob_specification):
        assert trait.BlobTrait(a_blob_specification).data() is a_blob_specification.data()

    def test_when_wrapping_arbitrary_data_then_returns_data(self):
        data = specification.SpecificationData()
        assert trait.BlobTrait(data).data() is data

    # TODO(DF): fix this
    # def test_when_wrapping_unsupported_spec_then_returns_None(self):
    #     spec = trait.BaseSpecification()
    #     assert trait.BlobTrait(spec).data() is None


class Test_BlobTrait_getURL:
    def test_when_data_has_no_url_then_returns_None(self, a_specification_data):
        assert trait.BlobTrait(a_specification_data).getUrl() is None

    def test_when_data_has_url_then_returns_url(self, a_blob_specifications_data):
        expected = "some://url"
        a_blob_specifications_data.setTraitProperty(trait.BlobTrait.traitId(), "url", expected)

        actual = trait.BlobTrait(a_blob_specifications_data).getUrl()

        assert actual == expected


class Test_BlobTrait_setURL:
    def test_when_url_is_a_str_then_url_is_added_to_dict(self, a_specification_data):
        expected = "some://url"

        trait.BlobTrait(a_specification_data).setUrl(expected)

        assert a_specification_data.getTraitProperty(trait.BlobTrait.traitId(), "url") == expected

    # TODO(DF): Support "unsetting" properties?

    def test_when_url_is_wrong_type_then_TypeError_is_raised(self, a_blob_specifications_data):
        with pytest.raises(TypeError):
            trait.BlobTrait(a_blob_specifications_data).setUrl(123)


class Test_BlobTrait_getMimeType:
    def test_when_data_has_no_mimeType_then_returns_None(self, a_specification_data):
        assert trait.BlobTrait(a_specification_data).getMimeType() is None

    def test_when_data_has_mimeType_then_returns_mimeType(self, a_blob_specifications_data):
        expected = "some://url"
        a_blob_specifications_data.setTraitProperty(
            trait.BlobTrait.traitId(), "mimeType", expected)

        actual = trait.BlobTrait(a_blob_specifications_data).getMimeType()

        assert actual == expected


class Test_BlobTrait_setMimeType:
    def test_when_mimeType_is_a_str_then_mimeType_is_added_to_dict(
            self, a_blob_specifications_data):
        expected = "application/x-something"

        trait.BlobTrait(a_blob_specifications_data).setMimeType(expected)

        assert a_blob_specifications_data.getTraitProperty(
            trait.BlobTrait.traitId(), "mimeType") == expected

    # TODO(DF): Support "unsetting" properties?

    def test_when_mimeType_is_wrong_type_then_TypeError_is_raised(
            self,
            a_blob_specifications_data):
        with pytest.raises(TypeError):
            trait.BlobTrait(a_blob_specifications_data).setMimeType(123)


@pytest.fixture
def a_blob_specifications_data(a_blob_specification):
    return a_blob_specification.data()


@pytest.fixture
def a_blob_specification():
    return specification.BlobSpecification()


@pytest.fixture
def a_specification_data():
    return specification.SpecificationData()
