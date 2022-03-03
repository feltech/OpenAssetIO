from typing import Union, Tuple


class BlobSpecification(BaseSpecification):
    kTraitIDs: Tuple[str]

    def blobTrait(self) -> BlobTrait:
        ...


class BaseSpecification:
    def traitIDs(self) -> Tuple[str]:
        ...

    def data(self) -> SpecificationData:
        ...


class SpecificationData:
    def __getitem__(self, traitName: str) -> Properties:
        ...

    def __len__(self) -> int:
        ...


class Properties:
    def __getitem__(self, key: str) -> Union[str, int, float, bool, SimpleMap]:
        ...

    def __setitem__(self, key: str, value: Union[str, int, float, bool, SimpleMap]) -> None:
        ...

    def __len__(self) -> int:
        ...


class SimpleMap:
    def __getitem__(self, key: str) -> Union[str, int, float, bool]:
        ...

    def __setitem__(self, key: str, value: Union[str, int, float, bool]) -> None:
        ...


class BlobTrait(BaseTrait):
    kID: str

    def getURL(self) -> str:
        ...

    def setURL(self, url: str) -> None:
        ...

    def getMimeType(self) -> str:
        ...

    def setMimeType(self, mimeType: str) -> None:
        ...


class BaseTrait:
    def __init__(self, data: Union[SpecificationData, BaseSpecification]):
        ...

    def data(self) -> SpecificationData:
        ...

    def isValid(self) -> bool:
        ...
