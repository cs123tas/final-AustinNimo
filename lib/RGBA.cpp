#include "RGBA.h"
#include "SupportMethods.h"

RGBA RGBA::operator+(const RGBA &that) const {
    return RGBA(this->r + that.r, this->g + that.g, this->b + that.b, this->a + that.a);
}

RGBA RGBA::operator-(const RGBA &that) const {
    return RGBA(this->r - that.r, this->g - that.g, this->b - that.b, this->a - that.a);
}

RGBA RGBA::operator*(const RGBA &that) const {
    return RGBA(this->r * that.r, this->g * that.g, this->b * that.b, this->a * that.a);
}

RGBA RGBA::operator/(const RGBA &that) const {
    return RGBA(this->r / that.r, this->g / that.g, this->b / that.b, this->a / that.a);
}

bool operator==(const RGBA &c1, const RGBA &c2) {
    return SupportMethods::fEquals(c1.r, c2.r, .1) &&
            SupportMethods::fEquals(c1.g, c2.g, .1) &&
            SupportMethods::fEquals(c1.b, c2.b, .1);
}

bool operator!=(const RGBA &c1, const RGBA &c2) {
    return !operator==(c1, c2);
}
