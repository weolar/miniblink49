/**************************************************************************\
*
* Copyright (c) 2000, Microsoft Corp.  All Rights Reserved.
*
* Module Name:
* 
*   GdiplusFontCollection.h
*
* Abstract:
*
*   Font collections (Installed and Private)
*
\**************************************************************************/

#ifndef _GDIPLUSFONTCOLL_H
#define _GDIPLUSFONTCOLL_H

inline
FontCollection::FontCollection()
{
    nativeFontCollection = NULL;
}

inline
FontCollection::~FontCollection()
{
}

inline INT
FontCollection::GetFamilyCount() const
{
    INT numFound = 0;

    lastResult = DllExports::GdipGetFontCollectionFamilyCount(
                             nativeFontCollection, &numFound);



    return numFound;
}

inline Status
FontCollection::GetFamilies(
    IN INT           numSought,
    OUT FontFamily * gpfamilies,
    OUT INT *        numFound
) const
{
    if (numSought <= 0 || gpfamilies == NULL || numFound == NULL)
    {
        return SetStatus(InvalidParameter);
    }
    *numFound = 0;
    GpFontFamily **nativeFamilyList = new GpFontFamily*[numSought];

    if (nativeFamilyList == NULL)
    {
        return SetStatus(OutOfMemory);
    }

    Status status = SetStatus(DllExports::GdipGetFontCollectionFamilyList(
        nativeFontCollection,
        numSought,
        nativeFamilyList,
        numFound
    ));
    if (status == Ok)
    {
        for (INT i = 0; i < *numFound; i++)
        {
            DllExports::GdipCloneFontFamily(nativeFamilyList[i],
                                            &gpfamilies[i].nativeFamily);
        }
    }

    delete [] nativeFamilyList;

    return status;
}

inline Status FontCollection::GetLastStatus () const
{
    return lastResult;
}

// protected method
inline Status
FontCollection::SetStatus(IN Status status) const
{
    lastResult = status;
    return lastResult;
}

inline
InstalledFontCollection::InstalledFontCollection()
{
    nativeFontCollection = NULL;
    lastResult = DllExports::GdipNewInstalledFontCollection(&nativeFontCollection);
}

inline
InstalledFontCollection::~InstalledFontCollection()
{
}

#ifndef DCR_USE_NEW_235072
inline Status
InstalledFontCollection::InstallFontFile(IN const WCHAR* filename)
{
    return SetStatus(DllExports::GdipInstallFontFile(nativeFontCollection, filename));
}

inline Status
InstalledFontCollection::UninstallFontFile(IN const WCHAR* filename)
{
    return SetStatus(DllExports::GdipUninstallFontFile(nativeFontCollection, filename));
}
#endif

inline
PrivateFontCollection::PrivateFontCollection()
{
    nativeFontCollection = NULL;
    lastResult = DllExports::GdipNewPrivateFontCollection(&nativeFontCollection);
}

inline
PrivateFontCollection::~PrivateFontCollection()
{
    DllExports::GdipDeletePrivateFontCollection(&nativeFontCollection);
}

inline Status
PrivateFontCollection::AddFontFile(IN const WCHAR* filename)
{
    return SetStatus(DllExports::GdipPrivateAddFontFile(nativeFontCollection, filename));
}

inline Status
PrivateFontCollection::AddMemoryFont(IN const void* memory,
                                     IN INT length)
{
    return SetStatus(DllExports::GdipPrivateAddMemoryFont(
        nativeFontCollection,
        memory,
        length));
}

#endif // _GDIPLUSFONTCOLL_H
