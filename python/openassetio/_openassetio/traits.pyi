from typing import Union, Tuple


class BlobSpecification(BaseSpecification):
    kTraits: Tuple[str]

    def blobTrait(self) -> BlobTrait:
        ...


class BaseSpecification:
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


class BlobTrait:
    kName: str

    def __init__(self, data: SpecificationData):
        ...

    def getURL(self) -> str:
        ...

    def setURL(self, url: str) -> None:
        ...

    def getMimeType(self) -> str:
        ...

    def setMimeType(self, mimeType: str) -> None:
        ...
