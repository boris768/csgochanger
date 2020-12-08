#pragma once

#include <cstdint>

#pragma warning(push)
#pragma warning(disable:4333 4244)
namespace SourceEngine
{
    class Color
    {
    public:
        // constructors
        Color() noexcept
        {
            *reinterpret_cast<int *>(this) = 0;
        }
		constexpr Color(const uint8_t r, const  uint8_t g, const  uint8_t b): _color{r,g,b,255}
        {
        }

        constexpr Color(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a): _color{ r,g,b,a }
        {
        }

        constexpr void SetColor(const uint8_t r, const  uint8_t g, const uint8_t b, const uint8_t a = 0) noexcept
        {
            _color[0] = static_cast<uint8_t>(r);
            _color[1] = static_cast<uint8_t>(g);
            _color[2] = static_cast<uint8_t>(b);
            _color[3] = static_cast<uint8_t>(a);
        }

        void GetColor(uint8_t &r, uint8_t &g, uint8_t &b, uint8_t &a) const noexcept
        {
            r = _color[0];
            g = _color[1];
            b = _color[2];
            a = _color[3];
        }

        static Color FromARGB(const unsigned long color) noexcept
        {
	        const int a = (int) ((color & 0xFF000000) >> 24);
	        const int r = (int) ((color & 0x00FF0000) >> 16);
			const int g = (int) ((color & 0x0000FF00) >> 8);
			const int b = (int) (color & 0x000000FF);

            return Color((uint8_t) r, (uint8_t) g, (uint8_t) b, (uint8_t) a);
        }

        void SetARGB(const unsigned long color) noexcept
        {
			const int a = (int) ((color & 0xFF000000) >> 24);
			const int r = (int) ((color & 0x00FF0000) >> 16);
			const int g = (int) ((color & 0x0000FF00) >> 8);
			const int b = (int) (color & 0x000000FF);

            SetColor((uint8_t) r, (uint8_t) g, (uint8_t) b, (uint8_t) a);
        }

	    unsigned long GetAsARGB() const noexcept
        {
			const int a = (_color[3]) >> 24;
			const int r = (_color[0]) >> 16;
			const int g = (_color[1]) >> 8;
			const int b = (_color[2]);

            return (unsigned long) (a | r | g | b);
        }

        uint8_t &operator[](size_t index) noexcept
        {
            return _color[index];
        }

        const uint8_t &operator[](size_t index) const noexcept
        {
            return _color[index];
        }

        bool operator == (const Color &rhs) const noexcept
        {
            return (*((int *)this) == *((int *)&rhs));
        }

        bool operator != (const Color &rhs) const noexcept
        {
            return !(operator==(rhs));
        }

        int r() { return _color[0]; }
        int g() { return _color[1]; }
        int b() { return _color[2]; }
        int a() { return _color[3]; }

        int GetR() { return _color[0]; }
        int GetG() { return _color[1]; }
        int GetB() { return _color[2]; }
        int GetA() { return _color[3]; }

        void SetR(uint8_t _i) { _color[0] = _i; }
        void SetG(uint8_t _i) { _color[1] = _i; }
        void SetB(uint8_t _i) { _color[2] = _i; }
        void SetA(uint8_t _i) { _color[3] = _i; }

        /*Color &operator=(const Color &rhs)
        {
            *(int*)(&_color[0]) = *(int*)&rhs._color[0];
            return *this;
        }*/

        Color operator+(const Color &rhs) const noexcept
        {
            int red = _color[0] + rhs._color[0];
            int green = _color[1] + rhs._color[1];
            int blue = _color[2] + rhs._color[2];
            int alpha = _color[3] + rhs._color[3];

            red = red > 255 ? 255 : red;
            green = green > 255 ? 255 : green;
            blue = blue > 255 ? 255 : blue;
            alpha = alpha > 255 ? 255 : alpha;

            return Color((uint8_t) red, (uint8_t) green, (uint8_t) blue, (uint8_t) alpha);
        }

        Color operator-(const Color &rhs) const noexcept
        {
            int red = _color[0] - rhs._color[0];
            int green = _color[1] - rhs._color[1];
            int blue = _color[2] - rhs._color[2];
            int alpha = _color[3] - rhs._color[3];

            red = red < 0 ? 0 : red;
            green = green < 0 ? 0 : green;
            blue = blue < 0 ? 0 : blue;
            alpha = alpha < 0 ? 0 : alpha;
            return Color((uint8_t) red, (uint8_t) green, (uint8_t) blue, (uint8_t) alpha);
        }

        operator const uint8_t*() const noexcept
        {
            return &_color[0];
        }

    private:
        uint8_t _color[4];
    };
}
#pragma warning(pop)