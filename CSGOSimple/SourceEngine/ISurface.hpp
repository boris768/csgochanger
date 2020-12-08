#pragma once

#include "Definitions.hpp"
#include "Vector2D.hpp"
#include "Color.hpp"
#include "IAppSystem.hpp"
#include "../IMaterialSystem.h"
#include <memory>

struct ImVec2;

namespace SourceEngine
{
    typedef ULONG HFont;
	class IMesh;

    struct IntRect
    {
        int x0;
        int y0;
        int w;
        int h;
    };

	struct Vertex_t
	{
		Vertex_t() = default;

		explicit Vertex_t( Vector2D pos, Vector2D coord = Vector2D(0, 0));
		void Init(Vector2D pos, Vector2D coord = Vector2D(0, 0));
		void Init(const ImVec2& pos, const ImVec2& coord);
		Vector2D m_Position;
		Vector2D m_TexCoord;
	};
	class TextureHolder
	{
	public:
		typedef std::shared_ptr<TextureHolder> Ptr;
		TextureHolder();
		TextureHolder(const unsigned char* pRawRGBAData, int W, int H);
		TextureHolder(const char* filename, int W, int H);
		~TextureHolder();
		[[nodiscard]] bool IsValid() const;
		[[nodiscard]] int GetTextureId() const;
		bool Draw(int x, int y, float scale = 1.0);

	private:
		int		m_iTexture;
		int		m_iW, m_iH;
		bool	m_bValid;
	};


#ifdef CreateFont
#undef CreateFont
#endif
    //-----------------------------------------------------------------------------
    // Purpose: Wraps contextless windows system functions
    //-----------------------------------------------------------------------------
    class ISurface : public IAppSystem
    {
    public:
        // frame
        virtual void        RunFrame() = 0;
        virtual VPANEL      GetEmbeddedPanel() = 0;
        virtual void        SetEmbeddedPanel(VPANEL pPanel) = 0;
        virtual void        PushMakeCurrent(VPANEL panel, bool useInsets) = 0;
        virtual void        PopMakeCurrent(VPANEL panel) = 0;
        virtual void        DrawSetColor(int r, int g, int b, int a) = 0;
        virtual void        DrawSetColor(Color col) = 0;
        virtual void        DrawFilledRect(int x0, int y0, int x1, int y1) = 0;
        virtual void        DrawFilledRectArray(IntRect *pRects, int numRects) = 0;
        virtual void        DrawOutlinedRect(int x0, int y0, int x1, int y1) = 0;
        virtual void        DrawLine(int x0, int y0, int x1, int y1) = 0;
        virtual void        DrawPolyLine(int *px, int *py, int numPoints) = 0;
        virtual void        DrawSetApparentDepth(float f) = 0;
        virtual void        DrawClearApparentDepth() = 0;
        virtual void        DrawSetTextFont(HFont font) = 0;
        virtual void        DrawSetTextColor(int r, int g, int b, int a) = 0;
        virtual void        DrawSetTextColor(Color col) = 0;
        virtual void        DrawSetTextPos(int x, int y) = 0;
        virtual void        DrawGetTextPos(int& x, int& y) = 0;
        virtual void        DrawPrintText(const wchar_t *text, int textLen, FontDrawType drawType = FontDrawType::FONT_DRAW_DEFAULT) = 0;
        virtual void        DrawUnicodeChar(wchar_t wch, FontDrawType drawType = FontDrawType::FONT_DRAW_DEFAULT) = 0;
        virtual void        DrawFlushText() = 0;
        virtual void*       CreateHTMLWindow(void *events, VPANEL context) = 0;
        virtual void        PaintHTMLWindow(void *htmlwin) = 0;
        virtual void        DeleteHTMLWindow(void *htmlwin) = 0;
        virtual int         DrawGetTextureId(char const *filename) = 0;
        virtual bool        DrawGetTextureFile(int id, char *filename, int maxlen) = 0;
        virtual void        DrawSetTextureFile(int id, const char *filename, int hardwareFilter, bool forceReload) = 0;
        virtual void        DrawSetTextureRGBA(int id, const unsigned char *rgba, int wide, int tall) = 0;
        virtual void        DrawSetTexture(int id) = 0;
        virtual void        DeleteTextureByID(int id) = 0;
        virtual void        DrawGetTextureSize(int id, int &wide, int &tall) = 0;
        virtual void        DrawTexturedRect(int x0, int y0, int x1, int y1) = 0;
        virtual bool        IsTextureIDValid(int id) = 0;
        virtual int         CreateNewTextureID(bool procedural = false) = 0;
        virtual void        GetScreenSize(int &wide, int &tall) = 0;
        virtual void        SetAsTopMost(VPANEL panel, bool state) = 0;
        virtual void        BringToFront(VPANEL panel) = 0;
        virtual void        SetForegroundWindow(VPANEL panel) = 0;
        virtual void        SetPanelVisible(VPANEL panel, bool state) = 0;
        virtual void        SetMinimized(VPANEL panel, bool state) = 0;
        virtual bool        IsMinimized(VPANEL panel) = 0;
        virtual void        FlashWindow(VPANEL panel, bool state) = 0;
        virtual void        SetTitle(VPANEL panel, const wchar_t *title) = 0;
        virtual void        SetAsToolBar(VPANEL panel, bool state) = 0;
        virtual void        CreatePopup(VPANEL panel, bool minimised, bool showTaskbarIcon = true, bool disabled = false, bool mouseInput = true, bool kbInput = true) = 0;
        virtual void        SwapBuffers(VPANEL panel) = 0;
        virtual void        Invalidate(VPANEL panel) = 0;
        virtual void        SetCursor(unsigned long cursor) = 0;
        virtual bool        IsCursorVisible() = 0;
        virtual void        ApplyChanges() = 0;
        virtual bool        IsWithin(int x, int y) = 0;
        virtual bool        HasFocus() = 0;
        virtual bool        SupportsFeature(int /*SurfaceFeature_t*/ feature) = 0;
        virtual void        RestrictPaintToSinglePanel(VPANEL panel, bool bForceAllowNonModalSurface = false) = 0;
        virtual void        SetModalPanel(VPANEL) = 0;
        virtual VPANEL      GetModalPanel() = 0;
        virtual void        UnlockCursor() = 0;
        virtual void        LockCursor() = 0;
        virtual void        SetTranslateExtendedKeys(bool state) = 0;
        virtual VPANEL      GetTopmostPopup() = 0;
        virtual void        SetTopLevelFocus(VPANEL panel) = 0;
        virtual HFont       CreateFont() = 0;
        virtual bool        SetFontGlyphSet(HFont font, const char *windowsFontName, int tall, int weight, int blur, int scanlines, int flags, int nRangeMin = 0, int nRangeMax = 0) = 0;
        virtual bool        AddCustomFontFile(const char *fontFileName) = 0;
        virtual int         GetFontTall(HFont font) = 0;
        virtual int         GetFontAscent(HFont font, wchar_t wch) = 0;
        virtual bool        IsFontAdditive(HFont font) = 0;
        virtual void        GetCharABCwide(HFont font, int ch, int &a, int &b, int &c) = 0;
        virtual int         GetCharacterWidth(HFont font, int ch) = 0;
        virtual void        GetTextSize(HFont font, const wchar_t *text, int &wide, int &tall) = 0;
        virtual VPANEL      GetNotifyPanel() = 0;
        virtual void        SetNotifyIcon(VPANEL context, ULONG icon, VPANEL panelToReceiveMessages, const char *text) = 0;
        virtual void        PlaySound(const char *fileName) = 0;
        virtual int         GetPopupCount() = 0;
        virtual VPANEL      GetPopup(int index) = 0;
        virtual bool        ShouldPaintChildPanel(VPANEL childPanel) = 0;
        virtual bool        RecreateContext(VPANEL panel) = 0;
        virtual void        AddPanel(VPANEL panel) = 0;
        virtual void        ReleasePanel(VPANEL panel) = 0;
        virtual void        MovePopupToFront(VPANEL panel) = 0;
        virtual void        MovePopupToBack(VPANEL panel) = 0;
        virtual void        SolveTraverse(VPANEL panel, bool forceApplySchemeSettings = false) = 0;
        virtual void        PaintTraverse(VPANEL panel) = 0;
        virtual void        EnableMouseCapture(VPANEL panel, bool state) = 0;
        virtual void        GetWorkspaceBounds(int &x, int &y, int &wide, int &tall) = 0;
        virtual void        GetAbsoluteWindowBounds(int &x, int &y, int &wide, int &tall) = 0;
        virtual void        GetProportionalBase(int &width, int &height) = 0;
        virtual void        CalculateMouseVisible() = 0;
        virtual bool        NeedKBInput() = 0;
        virtual bool        HasCursorPosFunctions() = 0;
        virtual void        SurfaceGetCursorPos(int &x, int &y) = 0;
        virtual void        SurfaceSetCursorPos(int x, int y) = 0;
        virtual void        DrawTexturedLine(const Vertex_t &a, const Vertex_t &b) = 0;
        virtual void        DrawOutlinedCircle(int x, int y, int radius, int segments) = 0;
        virtual void        DrawTexturedPolyLine(const Vertex_t *p, int n) = 0;
        virtual void        DrawTexturedSubRect(int x0, int y0, int x1, int y1, float texs0, float text0, float texs1, float text1) = 0;
        virtual void        DrawTexturedPolygon(int n, Vertex_t *pVertice, bool bClipVertices = true) = 0;
		void*	vtable1_pad;
		void*	vtable2_pad;
    	
    	// Point Translation for current panel
		int				m_nTranslateX;
		int				m_nTranslateY;

		// alpha multiplier for current panel [0..1]
		float			m_flAlphaMultiplier;

		char pad_0x0018[0xC]; //0x0018

		// The size of the window to draw into
		//int				m_pSurfaceExtents[4];
		__int32 m_nWidth; //0x0024 
		__int32 m_nHeight; //0x0028 

		// Color for drawing all non-text things
		unsigned char	m_DrawColor[4];

		// Color for drawing text
		unsigned char	m_DrawTextColor[4];

		// Location of text rendering
		int				m_pDrawTextPos[2];

		// Meshbuilder used for drawing
		IMesh* m_pMesh;
		CMeshBuilder meshBuilder;

		// White material used for drawing non-textured things
		CMaterialReference m_pWhite;
    };
	/*
	 *	__int32 m_nTranslateX; //0x000C 
	__int32 m_nTranslateY; //0x0010 
	float m_flAlphaMultiplier; //0x0014 
	char pad_0x0018[0xC]; //0x0018
	__int32 m_nWidth; //0x0024 
	__int32 m_nHeight; //0x0028 
	unsigned char r; //0x002C 
	unsigned char g; //0x002D 
	unsigned char b; //0x002E 
	unsigned char a; //0x002F 
	unsigned char N0000013F; //0x0030 
	unsigned char N0000024E; //0x0031 
	unsigned char N00000255; //0x0032 
	unsigned char N0000024F; //0x0033 
	__int32 N00000140; //0x0034 
	__int32 N00000141; //0x0038 
	N0000024B m_pMesh; //0x003C 
	char pad_0x0040[0x400]; //0x0040
	 *
	 *
	 */
}
