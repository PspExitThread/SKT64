#include "ImageLoader.h"


namespace ImageLoader
{
	NTSTATUS GkiImgLoadImage(HANDLE FileHandle, PVOID* ImageBase, PULONG ImageSize)
	{
		UNREFERENCED_PARAMETER(FileHandle);
		UNREFERENCED_PARAMETER(ImageBase);
		UNREFERENCED_PARAMETER(ImageSize);
		return 0;
	}
}