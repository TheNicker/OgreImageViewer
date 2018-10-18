#pragma once
#include <fstream>
#include <PlatformUtility.h>

struct BitmapBuffer
{
    const std::byte* buffer;
    uint8_t bitsPerPixel;
    uint32_t width;
    uint32_t height;
    uint32_t rowPitch;
};

class Bitmap;
using BitmapShaderPtr = std::shared_ptr< Bitmap>;

class Bitmap
{
    struct {
        BITMAPINFOHEADER bmiHeader;
        RGBQUAD bmiColors[256];
    } fBitmapInfo = {};

public:
    Bitmap(HBITMAP bitmap) : fBitmap(bitmap)
    {


    }



    const BITMAPINFOHEADER& GetBitmapHeader()
    {
        if (fBitmapInfo.bmiHeader.biSize == 0)
        {
            fBitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

            HDC hDC = GetDC(nullptr);

            GetDIBits(hDC, fBitmap, 0, 1, nullptr, (BITMAPINFO *)&fBitmapInfo,
                DIB_RGB_COLORS);
            ReleaseDC(nullptr, hDC);
        }
        return fBitmapInfo.bmiHeader;
    }



    static BitmapShaderPtr FromMemory(const BitmapBuffer& bitmapBuffer)
    {

        const int height = bitmapBuffer.height;
        const int width = bitmapBuffer.width;
        const int bpp = bitmapBuffer.bitsPerPixel;
        const int rowPitch = bitmapBuffer.rowPitch;

        BITMAPINFO bi;
        bi.bmiHeader = {};
        bi.bmiHeader.biBitCount = bpp;
        bi.bmiHeader.biClrImportant = 0;
        bi.bmiHeader.biClrUsed = 0;
        bi.bmiHeader.biCompression = 0;
        bi.bmiHeader.biHeight = height;
        bi.bmiHeader.biWidth = width;
        bi.bmiHeader.biPlanes = 1;
        bi.bmiHeader.biSize = 40;
        bi.bmiHeader.biSizeImage = rowPitch * height;

        bi.bmiHeader.biXPelsPerMeter = 11806;
        bi.bmiHeader.biYPelsPerMeter = 11806;


        bi.bmiColors[0].rgbBlue = 0;
        bi.bmiColors[0].rgbGreen = 0;
        bi.bmiColors[0].rgbRed = 0;
        bi.bmiColors[0].rgbReserved = 0;



        const std::byte* pPixels = (bitmapBuffer.buffer);

        char* ppvBits;

        HBITMAP hBitmap = CreateDIBSection(NULL, &bi, DIB_RGB_COLORS, (void**)&ppvBits, NULL, 0);
        int res = SetDIBits(NULL, hBitmap, 0, height, pPixels, &bi, DIB_RGB_COLORS);
        return std::make_shared<BitmapShaderPtr::element_type>(hBitmap);
    }


    static BitmapShaderPtr FromMemory(const std::wstring& filePath)
    {

        std::ifstream is;
        is.open(filePath, std::ios::binary);
        is.seekg(0, std::ios::end);
        size_t length = is.tellg();
        is.seekg(0, std::ios::beg);
        char* pBuffer = new char[length];
        is.read(pBuffer, length);
        is.close();



        const int height = 816;
        const int width = 822;
        const int bpp = 32;
        const int rowPitch = 3288;

        for (int y = 0; y < height; y++)
            for (int x = 0; x < width; x++)
            {
                size_t start = y * rowPitch + x * 4;
                //flip Red and blue channles.
                std::swap(pBuffer[start + 0], pBuffer[start + 2]);
            }

        char* flipped = new char[length];

        for (int y = 0; y < height; y++)
            memcpy(flipped + rowPitch * y, pBuffer + rowPitch * (height - y - 1), rowPitch);

        delete pBuffer;
        pBuffer = flipped;


        BITMAPINFO bi;
        bi.bmiHeader = {};
        bi.bmiHeader.biBitCount = bpp;
        bi.bmiHeader.biClrImportant = 0;
        bi.bmiHeader.biClrUsed = 0;
        bi.bmiHeader.biCompression = 0;
        bi.bmiHeader.biHeight = height;
        bi.bmiHeader.biWidth = width;
        bi.bmiHeader.biPlanes = 1;
        bi.bmiHeader.biSize = 40;
        bi.bmiHeader.biSizeImage = rowPitch * height;

        bi.bmiHeader.biXPelsPerMeter = 11806;
        bi.bmiHeader.biYPelsPerMeter = 11806;


        bi.bmiColors[0].rgbBlue = 0;
        bi.bmiColors[0].rgbGreen = 0;
        bi.bmiColors[0].rgbRed = 0;
        bi.bmiColors[0].rgbReserved = 0;



        char* pPixels = (pBuffer);

        char* ppvBits;

        HBITMAP hBitmap = CreateDIBSection(NULL, &bi, DIB_RGB_COLORS, (void**)&ppvBits, NULL, 0);
        int res = SetDIBits(NULL, hBitmap, 0, height, pPixels, &bi, DIB_RGB_COLORS);
        //std::string lastError = LLUtils::PlatformUtility::GetLastErrorAsString<char>();

        delete pBuffer;

        return std::make_shared<BitmapShaderPtr::element_type>(hBitmap);
    }

    static BitmapShaderPtr FromFileAnyFormat(const std::wstring& filePath)
    {
        std::ifstream is;
        is.open(filePath, std::ios::binary);
        is.seekg(0, std::ios::end);
        size_t length = is.tellg();
        is.seekg(0, std::ios::beg);
        char* pBuffer = new char[length];
        is.read(pBuffer, length);
        is.close();
        
         HBITMAP bmp = (HBITMAP) LoadImage(GetModuleHandle(nullptr), filePath.c_str() /*L"D:\\Cursor\\arrow-C.png"*/, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
         return std::make_shared<BitmapShaderPtr::element_type>(bmp);
        /*tagBITMAPFILEHEADER bfh = *(tagBITMAPFILEHEADER*)pBuffer;
        tagBITMAPINFOHEADER bih = *(tagBITMAPINFOHEADER*)(pBuffer + sizeof(tagBITMAPFILEHEADER));
        RGBQUAD             rgb = *(RGBQUAD*)(pBuffer + sizeof(tagBITMAPFILEHEADER) + sizeof(tagBITMAPINFOHEADER));

        BITMAPINFO bi;
        bi.bmiColors[0] = rgb;
        bi.bmiHeader = bih;

        char* pPixels = (pBuffer + bfh.bfOffBits);

        char* ppvBits;

        HBITMAP hBitmap = CreateDIBSection(NULL, &bi, DIB_RGB_COLORS, (void**)&ppvBits, NULL, 0);

        int res = SetDIBits(NULL, hBitmap, 0, bih.biHeight, pPixels, &bi, DIB_RGB_COLORS);

        return std::make_shared<BitmapShaderPtr::element_type>(hBitmap);*/
    }

    static BitmapShaderPtr FromFile(const std::wstring& filePath)
    {
        std::ifstream is;
        is.open(filePath, std::ios::binary);
        is.seekg(0, std::ios::end);
        size_t length = is.tellg();
        is.seekg(0, std::ios::beg);
        char* pBuffer = new char[length];
        is.read(pBuffer, length);
        is.close();

        tagBITMAPFILEHEADER bfh = *(tagBITMAPFILEHEADER*)pBuffer;
        tagBITMAPINFOHEADER bih = *(tagBITMAPINFOHEADER*)(pBuffer + sizeof(tagBITMAPFILEHEADER));
        RGBQUAD             rgb = *(RGBQUAD*)(pBuffer + sizeof(tagBITMAPFILEHEADER) + sizeof(tagBITMAPINFOHEADER));

        BITMAPINFO bi;
        bi.bmiColors[0] = rgb;
        bi.bmiHeader = bih;

        char* pPixels = (pBuffer + bfh.bfOffBits);

        char* ppvBits;

        HBITMAP hBitmap = CreateDIBSection(NULL, &bi, DIB_RGB_COLORS, (void**)&ppvBits, NULL, 0);

        int res = SetDIBits(NULL, hBitmap, 0, bih.biHeight, pPixels, &bi, DIB_RGB_COLORS);

        return std::make_shared<BitmapShaderPtr::element_type>(hBitmap);
    }

    HBITMAP GetHBitmap() const { return fBitmap; }

private:
    HBITMAP fBitmap;
};



class ImageList
{
public:
    struct ImageDesc
    {
        uint32_t index;
        std::wstring title;
        BitmapShaderPtr bitmap;
    };

    struct RGBAImageDesc
    {
        std::byte* buffer;
        uint32_t width;
        uint32_t height;
    };

public:
    void SetTarget(HWND hwnd)
    {
        fTargetWindow = hwnd;
    }


    void MouseClick(int xPos, int yPos)
    {
        int selected = yPos / fEntryHeight + fPos;
        if (selected <= fImages.size())
        {
            fSelected = selected;
        }
    }

    void SetPos(int pos)
    {
        fPos = pos;
    }

    int GetNumberOfDisplayedElements()
    {
        HWND hwnd = fTargetWindow;
        RECT rect;
        GetClientRect(hwnd, &rect);
        int height = rect.bottom - rect.top;

        const int numberOfMaxVisibleElements = height / fEntryHeight;
        return (std::min)(numberOfMaxVisibleElements, GetNumberOfElements());
    }

    int GetNumberOfElements()
    {
        return fImages.size();
    }


    void Draw()
    {
        RECT rect;
        HWND hwnd = fTargetWindow;
        GetClientRect(hwnd, &rect);
        const int imageDestWidth = 64;
        const int imageDestHeight = 64;
        const int entrywidth = rect.right - rect.left;
        int imagePos = 30;

        PAINTSTRUCT ps;

        BeginPaint(hwnd, &ps);
        HDC hdc = ps.hdc;// GetDC(hwnd);//  BeginPaint(GetHandleClient(), &ps);


        HDC hdcMem = CreateCompatibleDC(nullptr);

        HBRUSH grayBrush = CreateSolidBrush(RGB(245, 249, 213));
        HBRUSH lightgrayBrush = CreateSolidBrush(RGB(224, 249, 213));
        HBRUSH greenBrush = CreateSolidBrush(RGB(0, 255, 0));
        HBRUSH BlueBrush = CreateSolidBrush(RGB(0, 0, 200));
        const int lineWidth = 2;
        HPEN hPen = CreatePen(PS_DASHDOTDOT, lineWidth, RGB(0, 0, 0));
        SelectObject(hdc, hPen);




        LOGFONT font = { 0 };

        font.lfHeight = 20;
        font.lfWeight = FW_NORMAL;
        wcscpy_s(font.lfFaceName, L"Segoe UI");

        // Draw the text directly to compare to the bitmap
        font.lfQuality = PROOF_QUALITY;
        HFONT hFont = CreateFontIndirect(&font);
        font.lfQuality = 0;
        HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);


        //LOGFONT lf = *plf;
        //lf.lfQuality = CLEARTYPE_QUALITY;
        //HFONT hFont = CreateFontIndirect(&lf);
        //HFONT hOldFont = (HFONT)SelectObject(hDC, hFont);


        int currentEntry = 0;
        int x = 0;
        int y = fPos * -100;
        for (const ImageDesc& imageDesc : fImages)
        {


            BITMAP bm;
            PAINTSTRUCT ps;

            HBITMAP currentBitmap = imageDesc.bitmap->GetHBitmap();

            HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, currentBitmap);
            GetObject(currentBitmap, sizeof(bm), &bm);
            RECT r;
            r.top = y;
            r.bottom = y + fEntryHeight;
            r.left = 0;
            r.right = entrywidth;

            if (currentEntry == fSelected)
                FillRect(hdc, &r, BlueBrush);
            else
                FillRect(hdc, &r, currentEntry % 2 == 0 ? grayBrush : lightgrayBrush);
            MoveToEx(hdc, x, y + fEntryHeight - lineWidth, nullptr);

            int textpos = 5 + y;

            LineTo(hdc, entrywidth, y + fEntryHeight - lineWidth);
            {
                RECT r = { 0, textpos ,0, textpos + 24 };
                std::wstring text = imageDesc.title;
                DrawText(hdc, text.c_str(), text.length(), &r, DT_CALCRECT);
                SetBkMode(hdc, TRANSPARENT);
                int offset = (entrywidth - (r.right - r.left)) / 2;
                r.right += offset;
                r.left += offset;
                DrawText(hdc, text.c_str(), text.length(), &r, DT_CENTER);
            }

            SetStretchBltMode(hdc, STRETCH_HALFTONE);







            int finalWidth = std::min<int>(imageDestWidth, imageDesc.bitmap->GetBitmapHeader().biWidth);
            int finalHeight = std::min<int>(imageDestHeight, imageDesc.bitmap->GetBitmapHeader().biHeight);


            int imageFinalLocalYPos = finalHeight < imageDestHeight ? (fEntryHeight - finalHeight) / 2 : imagePos;


            StretchBlt(hdc, (entrywidth - finalWidth) / 2, y + imageFinalLocalYPos, finalWidth, finalHeight, hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
            //BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);



            currentEntry++;
            y += fEntryHeight;
            SelectObject(hdcMem, hbmOld);
        }


        DeleteDC(hdcMem);
        //ReleaseDC(hwnd, hdc);
        EndPaint(hwnd, &ps);
    }

    void SetImage(const ImageDesc& imageDesc)
    {
        fImages.resize(imageDesc.index + 1);
        fImages[imageDesc.index] = imageDesc;
    }

    static  HBITMAP HBitmapFromMemory()
    {
        //400x240x24

        std::ifstream is;
        is.open("d:\\ImagesCopy\\loading.bmp", std::ios::binary);
        is.seekg(0, std::ios::end);
        size_t length = is.tellg();
        is.seekg(0, std::ios::beg);
        char* pBuffer = new char[length];
        is.read(pBuffer, length);
        is.close();

        tagBITMAPFILEHEADER bfh = *(tagBITMAPFILEHEADER*)pBuffer;
        tagBITMAPINFOHEADER bih = *(tagBITMAPINFOHEADER*)(pBuffer + sizeof(tagBITMAPFILEHEADER));
        RGBQUAD             rgb = *(RGBQUAD*)(pBuffer + sizeof(tagBITMAPFILEHEADER) + sizeof(tagBITMAPINFOHEADER));

        BITMAPINFO bi;
        bi.bmiColors[0] = rgb;
        bi.bmiHeader = bih;




        char* pPixels = (pBuffer + bfh.bfOffBits);

        char* ppvBits;


        const uint32_t width = 400;
        const uint32_t height = 240;

        tagBITMAPINFOHEADER bihRGBA = {};
        bihRGBA.biSize = sizeof(bihRGBA);
        bihRGBA.biWidth = width;
        bihRGBA.biHeight = height;
        bihRGBA.biPlanes = 1;
        bihRGBA.biBitCount = 32;
        bihRGBA.biSizeImage = width * height * 32;
        bihRGBA.biXPelsPerMeter = 11808;
        bihRGBA.biYPelsPerMeter = 11808;

        BITMAPINFO bi32;
        bi32.bmiColors[0] = rgb;
        bi32.bmiHeader = bihRGBA;

        HBITMAP hBitmap = CreateDIBSection(NULL, &bi, DIB_RGB_COLORS, (void**)&ppvBits, NULL, 0);
        SetDIBits(NULL, hBitmap, 0, bih.biHeight, pPixels, &bi, DIB_RGB_COLORS);

        const uint8_t* pixels32 = new uint8_t[width * height * 4];

        uint32_t rowPitch = LLUtils::Utility::Align<uint32_t>(bih.biWidth * (bih.biBitCount / 8), 4);

        uint8_t* sourceLine = (uint8_t*)pPixels;
        uint8_t* destLine = (uint8_t*)pixels32;

        uint32_t currentPixelInLine = 0;
        for (uint32_t line = 0; line < bih.biHeight; line++)
        {
            currentPixelInLine = 0;
            while (currentPixelInLine < bih.biWidth)
            {
                uint8_t* sourcePIxel = sourceLine + (currentPixelInLine * 3);

                ((uint32_t*)destLine)[currentPixelInLine] = (0x55 << 24) | (sourcePIxel[2] << 16) | (sourcePIxel[1] << 8) | sourcePIxel[0];
                //((uint32_t*)destLine)[currentPixelInLine] = 0xffffffff;
                currentPixelInLine++;
            }
            sourceLine += rowPitch;
            destLine += width * 4;
        }

        HBITMAP hBitmap32 = CreateDIBSection(NULL, &bi32, DIB_RGB_COLORS, (void**)&ppvBits, NULL, 0);
        SetDIBits(NULL, hBitmap32, 0, bihRGBA.biHeight, pixels32, &bi32, DIB_RGB_COLORS);

        //GetObject(hBitmap, sizeof(BITMAP), &cBitmap);
        //return hBitmap32;
        return hBitmap;
    }

private:
    uint32_t fEntryHeight = 100;
    int fSelected = -1;
    int fPos = 0;
    HWND fTargetWindow;
    std::vector<ImageDesc> fImages;
};





