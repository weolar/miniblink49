
bool saveDumpFile(const String& url, char* buffer, unsigned int size)
{
    HANDLE hFile = CreateFileA(url.utf8().data(),
        GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if (INVALID_HANDLE_VALUE != hFile) {
        DWORD dwSize = 0;
        WriteFile(hFile, buffer, size, &dwSize, NULL);
        CloseHandle(hFile);
        return TRUE;
    }
    return FALSE;
}

BOOL WriteBmp(const std::wstring &strFile, const std::vector<BYTE> &vtData, const SIZE &sizeImg);
BOOL WriteBmp(const std::wstring &strFile, HDC hdc);
BOOL WriteBmp(const std::wstring &strFile, HDC hdc, const RECT &rcDC);

BOOL WriteBmp(const std::wstring &strFile, const std::vector<BYTE> &vtData, const SIZE &sizeImg) {
    BITMAPINFOHEADER bmInfoHeader = { 0 };
    bmInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmInfoHeader.biWidth = sizeImg.cx;
    bmInfoHeader.biHeight = sizeImg.cy;
    bmInfoHeader.biPlanes = 1;
    bmInfoHeader.biBitCount = 24;

    //Bimap file header in order to write bmp file  
    BITMAPFILEHEADER bmFileHeader = { 0 };
    bmFileHeader.bfType = 0x4d42;  //bmp    
    bmFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    bmFileHeader.bfSize = bmFileHeader.bfOffBits + ((bmInfoHeader.biWidth * bmInfoHeader.biHeight) * 3); ///3=(24 / 8)  

    HANDLE hFile = CreateFile(strFile.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        return FALSE;
    }

    DWORD dwWrite = 0;
    WriteFile(hFile, &bmFileHeader, sizeof(BITMAPFILEHEADER), &dwWrite, NULL);
    WriteFile(hFile, &bmInfoHeader, sizeof(BITMAPINFOHEADER), &dwWrite, NULL);
    WriteFile(hFile, &vtData[0], vtData.size(), &dwWrite, NULL);


    CloseHandle(hFile);

    return TRUE;
}

BOOL WriteBmp(const std::wstring &strFile, HDC hdc) {
    int iWidth = GetDeviceCaps(hdc, HORZRES);
    int iHeight = GetDeviceCaps(hdc, VERTRES);
    RECT rcDC = { 0,0,iWidth,iHeight };

    return WriteBmp(strFile, hdc, rcDC);
}

BOOL WriteBmp(const std::wstring &strFile, HDC hdc, const RECT &rcDC) {
    BOOL bRes = FALSE;
    BITMAPINFO bmpInfo = { 0 };
    BYTE *pData = NULL;
    SIZE sizeImg = { 0 };
    HBITMAP hBmp = NULL;
    std::vector<BYTE> vtData;
    HGDIOBJ hOldObj = NULL;
    HDC hdcMem = NULL;

    //Initilaize the bitmap information   
    bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmpInfo.bmiHeader.biWidth = rcDC.right - rcDC.left;
    bmpInfo.bmiHeader.biHeight = rcDC.bottom - rcDC.top;
    bmpInfo.bmiHeader.biPlanes = 1;
    bmpInfo.bmiHeader.biBitCount = 24;

    //Create the compatible DC to get the data  
    hdcMem = CreateCompatibleDC(hdc);
    if (hdcMem == NULL) {
        goto EXIT;
    }

    //Get the data from the memory DC     
    hBmp = CreateDIBSection(hdcMem, &bmpInfo, DIB_RGB_COLORS, reinterpret_cast<VOID **>(&pData), NULL, 0);
    if (hBmp == NULL) {
        goto EXIT;
    }
    hOldObj = SelectObject(hdcMem, hBmp);

    //Draw to the memory DC  
    sizeImg.cx = bmpInfo.bmiHeader.biWidth;
    sizeImg.cy = bmpInfo.bmiHeader.biHeight;
    StretchBlt(hdcMem, 0, 0, sizeImg.cx, sizeImg.cy, hdc, rcDC.left,
        rcDC.top, rcDC.right - rcDC.left + 1, rcDC.bottom - rcDC.top + 1, SRCCOPY);

    vtData.resize(sizeImg.cx * sizeImg.cy * 3);
    memcpy(&vtData[0], pData, vtData.size());
    bRes = WriteBmp(strFile, vtData, sizeImg);

    SelectObject(hdcMem, hOldObj);

EXIT:
    if (hBmp != NULL) {
        DeleteObject(hBmp);
    }

    if (hdcMem != NULL) {
        DeleteDC(hdcMem);
    }

    return bRes;
}

#if 1
SkPaint paintTest;
//const SkColor color = 0xff000000 | (rand() % 3) * (rand() % 7) * GetTickCount();
const SkColor color = 0xffFF34F1;
paintTest.setColor(color);
paintTest.setStrokeWidth(4);
paintTest.setTextSize(13);
paintTest.setTextEncoding(SkPaint::kUTF8_TextEncoding);

//     CompositingLayer* layer24 = getCCLayerById(24);
//     if (layer24) {
//         String textTest = String::format("id:%d child:%d", 24, layer24->children().size());
//         CString cText = textTest.utf8();
//         m_memoryCanvas->drawText(cText.data(), cText.length(), 5, 35, paintTest);
//     }
// 
//     skia::BitmapPlatformDevice* device = (skia::BitmapPlatformDevice*)skia::GetPlatformDevice(skia::GetTopDevice(*m_memoryCanvas));
//     if (device) {
//         HDC hDC = device->GetBitmapDCUgly();
//         COLORREF c = ::GetPixel(hDC, 6, 3);
//         if (c == 0x00ffffff || c == 0xffffffff) {
//             if (0) {
//                 WriteBmp(L"E:\\mycode\\miniblink49\\trunk\\out\\DCSave.bmp", hDC);
//             }
//         }
//     }
#endif

void test1(const IntRect& dirtyRect)
{
    if (/*24 == m_layer->id() &&*/ dirtyRect.width() > 8 && dirtyRect.height() > 8) {
        SkBitmap* bitmap = new SkBitmap;
        bitmap->allocN32Pixels(dirtyRect.width(), dirtyRect.height());

        SkImageInfo info = SkImageInfo::MakeN32(dirtyRect.width(), dirtyRect.height(), kOpaque_SkAlphaType); // TODO
        SkSurfaceProps surfaceProps(0, kUnknown_SkPixelGeometry);
        size_t stride = info.minRowBytes();
        skia::RefPtr<SkSurface> surface = skia::AdoptRef(SkSurface::NewRasterDirect(info, bitmap->getPixels(), stride, &surfaceProps));
        skia::RefPtr<SkCanvas> canvas = skia::SharePtr(surface->getCanvas());

        SkPaint paint;
        paint.setAntiAlias(false);

        canvas->save();
        canvas->scale(1, 1);
        canvas->translate(-dirtyRect.x(), -dirtyRect.y());
        canvas->drawPicture(picture, nullptr, &paint);
        canvas->restore();

        uint32_t* c = bitmap->getAddr32(6, 3);
        if (*c == 0x00ffffff || *c == 0xffffffff) {
            taskGroup->isAlwaysWhite *= 1;
            isLayer24White = true;
        } else {
            taskGroup->isAlwaysWhite *= 0;
        }

        if (0) {
            Vector<unsigned char> output;
            blink::GDIPlusImageEncoder::encode(*bitmap, blink::GDIPlusImageEncoder::PNG, &output);
            cc::saveDumpFile("E:\\mycode\\miniblink49\\trunk\\out\\1.png", (char*)output.data(), output.size());
        }

        delete bitmap;
    }
}