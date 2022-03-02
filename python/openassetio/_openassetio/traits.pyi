

from typing import Union


class SpecificationData:
    def __getitem__(self, traitName: str) -> Properties:
        ...


class Properties:
    def __getitem__(self, key: str) -> Union[str, int, float, bool, SimpleMap, None]:
        ...

    def __setitem__(self, key: str, value: Union[str, int, float, bool, SimpleMap, None]) -> None:
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
