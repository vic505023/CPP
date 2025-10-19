#include "number.h"
#include <cstring>

int2025_t from_int(int32_t i) {
    int2025_t res{};
    uint32_t abs_value;
    if (i < 0) {
        res.is_negative = true;
        abs_value = static_cast<uint32_t>(-static_cast<int64_t>(i));
    }
    else {
        abs_value = static_cast<uint32_t>(i);
    }
    for (int j = 0; j < 4; j++) {
        res.data[j] = (abs_value >> (j * 8)) & 0xFF;
    }
    return res;
}

int2025_t from_string(const char* buff) {
    int2025_t res{};
    int i = 0;
    bool negative = false;
    if (buff[0] == '-') {
        negative = true;
        i++;
    }
    else if (buff[0] == '+') {
        i++;
    }
    const int2025_t kTen = from_int(10);
    while (buff[i] != '\0') {
        int2025_t symb = from_int(buff[i] - '0');
        res = res * kTen + symb;
        i++;
    }
    bool is_zero = true;
    for (int j = 0; j < 253; j++) {
        if (res.data[j] != 0) {
            is_zero = false;
            break;
        }
    }
    if (is_zero) {
        res.is_negative = false;
    }
    else {
        res.is_negative = negative;
    }
    return res;
}

int2025_t operator+(const int2025_t& lhs, const int2025_t& rhs) {
    int2025_t res{};
    if (lhs.is_negative == rhs.is_negative) {
        uint16_t carry = 0;
        for (int i = 0; i < 253; ++i) {
            uint16_t sum = lhs.data[i] + rhs.data[i] + carry;
            res.data[i] = sum & 0xFF;
            carry = sum >> 8;
        }
        res.is_negative = lhs.is_negative;
    } 
    else {
        bool lhs_bigger, equal = true;
        for (int i = 252; i >= 0; i--) {
            if (lhs.data[i] != rhs.data[i]) {
                lhs_bigger = (lhs.data[i] > rhs.data[i]);
                equal = false;
                break;
            }
        }
        if (equal) {
            res.is_negative = false;
        }
        else if (lhs_bigger) {
            int16_t hold = 0;
            for (int i = 0; i < 253; i++) {
                int16_t dif = lhs.data[i] - rhs.data[i] - hold;
                if (dif < 0) {
                    dif += 256;
                    hold = 1;
                }
                else {
                    hold = 0;
                }
                res.data[i] = dif & 0xFF;
            }
            res.is_negative = lhs.is_negative;
        }
        else {
            int16_t hold = 0;
            for (int i = 0; i < 253; i++) {
                int16_t dif = rhs.data[i] - lhs.data[i] - hold;
                if (dif < 0) {
                    dif += 256;
                    hold = 1;
                }
                else {
                    hold = 0;
                }
                res.data[i] = dif & 0xFF;
            }
            res.is_negative = rhs.is_negative;
        }
    }
    return res;
}

int2025_t operator-(const int2025_t& lhs, const int2025_t& rhs) {
    int2025_t neg_rhs = rhs;
    neg_rhs.is_negative = !rhs.is_negative;
    return lhs + neg_rhs;
}

int2025_t operator*(const int2025_t& lhs, const int2025_t& rhs) {
    int2025_t res{};
    if (lhs == res || rhs == res) return res;
    else {
        if (lhs.is_negative == rhs.is_negative) {
            res.is_negative = false;
        }
        else {
            res.is_negative = true;
        }
        uint16_t interim_res[506]{};
        for (int i = 0; i < 253; i++) {
            uint16_t carry = 0;
            for (int j = 0; j < 253; j++) {
                interim_res[i + j] += static_cast<uint16_t>(lhs.data[i]) * static_cast<uint16_t>(rhs.data[j]) + carry;
                carry = interim_res[i + j] >> 8;
                interim_res[i + j] &= 0xFF;
            }
            interim_res[i + 253] = carry;
        }
        for (int i = 0; i < 253; i++) {
            res.data[i] = interim_res[i];
        }
    }
    return res;
}

int2025_t operator/(const int2025_t& lhs, const int2025_t& rhs) {
    int2025_t res{};
    if (lhs == res || rhs == res) return res;
    else {
        bool lhs_bigger = true;
        for (int i = 252; i >= 0; i--) {  // Исправлено: 252 вместо 253
            if (lhs.data[i] != rhs.data[i]) {
                lhs_bigger = (lhs.data[i] > rhs.data[i]);
                break;
            }
        }
        if (!lhs_bigger && !(lhs == rhs)) return res;
        uint8_t current[253]{};
        uint8_t buff[253] = {0};
        uint8_t divider[253];
        std::memcpy(divider, rhs.data, 253);
        for (int pos = 2023; pos >= 0; pos--) {
            uint16_t carry = 0;
            for (int i = 0; i < 253; i++) {
                uint16_t val = (static_cast<uint16_t>(current[i]) << 1) | carry;
                current[i] = val & 0xFF;
                carry = val >> 8;
            }
            int byte_index = pos / 8;
            int bit_index = pos % 8;
            current[0] |= (lhs.data[byte_index] >> bit_index) & 1;
            bool can_subtract = true;
            for (int i = 252; i >= 0; i--) {
                if (current[i] != divider[i]) {
                    can_subtract = (current[i] > divider[i]);
                    break;
                }
            }
            if (can_subtract) {
                int16_t borrow = 0;
                for (int i = 0; i < 253; i++) {
                    int16_t diff = current[i] - divider[i] - borrow;
                    if (diff < 0) {
                        diff += 256;
                        borrow = 1;
                    }
                    else {
                        borrow = 0;
                    }
                    buff[i] = diff & 0xFF;
                }
                std::memcpy(current, buff, 253);
                res.data[pos / 8] |= (1 << (pos % 8));
            }
        }
        res.is_negative = (lhs.is_negative != rhs.is_negative);
    }
    return res;
}

bool operator==(const int2025_t& lhs, const int2025_t& rhs) {
    if (rhs.is_negative == lhs.is_negative) {
        for (int i = 0; i < 253; i++) {
            if (lhs.data[i] != rhs.data[i]) {
                return false;
            }
        }
        return true;
    }
    else return false;
}

bool operator!=(const int2025_t& lhs, const int2025_t& rhs) {
    return !(lhs == rhs);
}

std::ostream& operator<<(std::ostream& stream, const int2025_t& value) {
    bool is_zero = true;
    for (int i = 0; i < 253; ++i) {
        if (value.data[i] != 0) {
            is_zero = false;
            break;
        }
    }
    if (value.is_negative && !is_zero) {
        stream << '-';
    }
    bool found_nonzero = false;
    for (int i = 252; i >= 0; --i) {
        uint8_t byte = value.data[i];
        if (byte != 0 || found_nonzero) {
            if (!found_nonzero) {
                stream << static_cast<int>(byte);
                found_nonzero = true;
            } else {
                if (byte < 16) {
                    stream << '0';
                }
                stream << static_cast<int>(byte);
            }
        }
    }
    if (!found_nonzero) {
        stream << '0';
    }
    return stream;
}