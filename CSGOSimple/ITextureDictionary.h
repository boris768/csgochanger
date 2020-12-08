#pragma once
#include "IMaterial.h"

namespace SourceEngine
{
	class ITextureDictionary
	{
	public:
		// Create, destroy textures
		virtual int	CreateTexture(bool procedural = false) = 0;
		virtual void DestroyTexture(int id) = 0;
		virtual void DestroyAllTextures() = 0;

		// Is this a valid id?
		virtual bool IsValidId(int id) const = 0;

		// Binds a material to a texture
		virtual void BindTextureToFile(int id, const char *pFileName) = 0;

		// Binds a material to a texture
		virtual void BindTextureToMaterial(int id, IMaterial *pMaterial) = 0;

		// Binds a material to a texture
		virtual void BindTextureToMaterialReference(int id, int referenceId, IMaterial *pMaterial) = 0;

		// Texture info
		virtual IMaterial *GetTextureMaterial(int id) = 0;
		virtual void* GetTextureHandle(int id) = 0;
		virtual void GetTextureSize(int id, int& iWide, int& iTall) = 0;
		virtual void GetTextureTexCoords(int id, float &s0, float &t0, float &s1, float &t1) = 0;

		virtual void SetTextureRGBA(int id, const char* rgba, int wide, int tall) = 0;
		virtual void SetTextureRGBAEx(int id, const char* rgba, int wide, int tall, ImageFormat format) = 0;

		virtual int	FindTextureIdForTextureFile(char const *pFileName) = 0;
		virtual void SetSubTextureRGBA(int id, int drawX, int drawY, unsigned const char *rgba, int subTextureWide, int subTextureTall) = 0;
		virtual void SetSubTextureRGBAEx(int id, int drawX, int drawY, unsigned const char *rgba, int subTextureWide, int subTextureTall, ImageFormat fmt) = 0;
		virtual void UpdateSubTextureRGBA(int, int, int, unsigned const char *, int, int, ImageFormat) = 0;
	};
}