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

// ДАЛЬШЕ НЕ ОТРЕДАЧЕННЫЕ ФУНКЦИИ (НО РАБОТАЮЩИЕ)
int2025_t operator*(const int2025_t& lhs, const int2025_t& rhs) {
    bool lhs_zero = true;
    bool rhs_zero = true;
    for (int i = 0; i < 253; ++i) {
        if (lhs.data[i] != 0) lhs_zero = false;
        if (rhs.data[i] != 0) rhs_zero = false;
    }
    if (lhs_zero || rhs_zero) {
        return from_int(0);
    }
    int2025_t result{};
    uint16_t product[506] = {0};
    for (int i = 0; i < 253; ++i) {
        uint16_t carry = 0;
        for (int j = 0; j < 253; ++j) {
            if (i + j < 506) {
                product[i + j] += static_cast<uint16_t>(lhs.data[i]) * rhs.data[j] + carry;
                carry = product[i + j] >> 8;
                product[i + j] &= 0xFF;
            }
        }
        if (i + 253 < 506) {
            product[i + 253] = carry;
        }
    }
    for (int i = 0; i < 253; ++i) {
        result.data[i] = product[i] & 0xFF;
    }
    result.is_negative = lhs.is_negative != rhs.is_negative;
    bool is_zero = true;
    for (int i = 0; i < 253; ++i) {
        if (result.data[i] != 0) {
            is_zero = false;
            break;
        }
    }
    if (is_zero) {
        result.is_negative = false;
    }
    return result;
}

int2025_t operator/(const int2025_t& lhs, const int2025_t& rhs) {
    bool rhs_zero = true;
    for (int i = 0; i < 253; ++i) {
        if (rhs.data[i] != 0) {
            rhs_zero = false;
            break;
        }
    }
    if (rhs_zero) {
        return from_int(0);
    }
    bool lhs_zero = true;
    for (int i = 0; i < 253; ++i) {
        if (lhs.data[i] != 0) {
            lhs_zero = false;
            break;
        }
    }
    if (lhs_zero) {
        return from_int(0);
    }
    int cmp = 0;
    for (int i = 252; i >= 0; --i) {
        if (lhs.data[i] != rhs.data[i]) {
            cmp = (lhs.data[i] < rhs.data[i]) ? -1 : 1;
            break;
        }
    }
    if (cmp < 0) {
        return from_int(0);
    }
    int2025_t result{};
    uint8_t remainder[253] = {0};
    uint8_t current[253] = {0};
    uint8_t temp[253] = {0};
    uint8_t b_copy[253];
    std::memcpy(b_copy, rhs.data, 253);
    for (int i = 2023; i >= 0; --i) {
        uint16_t carry = 0;
        for (int j = 0; j < 253; ++j) {
            uint16_t val = (static_cast<uint16_t>(current[j]) << 1) | carry;
            current[j] = val & 0xFF;
            carry = val >> 8;
        }
        current[0] |= (lhs.data[i / 8] >> (i % 8)) & 1;
        int current_cmp = 0;
        for (int j = 252; j >= 0; --j) {
            if (current[j] != b_copy[j]) {
                current_cmp = (current[j] < b_copy[j]) ? -1 : 1;
                break;
            }
        }
        if (current_cmp >= 0) {
            int16_t borrow = 0;
            for (int j = 0; j < 253; ++j) {
                int16_t diff = current[j] - b_copy[j] - borrow;
                if (diff < 0) {
                    diff += 256;
                    borrow = 1;
                } else {
                    borrow = 0;
                }
                temp[j] = diff & 0xFF;
            }
            std::memcpy(current, temp, 253);
            result.data[i / 8] |= (1 << (i % 8));
        }
    }
    result.is_negative = lhs.is_negative != rhs.is_negative;
    bool is_zero = true;
    for (int i = 0; i < 253; ++i) {
        if (result.data[i] != 0) {
            is_zero = false;
            break;
        }
    }
    if (is_zero) {
        result.is_negative = false;
    }
    return result;
}

bool operator==(const int2025_t& lhs, const int2025_t& rhs) {
    if (lhs.is_negative != rhs.is_negative) {
        return false;
    }
    for (int i = 0; i < 253; ++i) {
        if (lhs.data[i] != rhs.data[i]) {
            return false;
        }
    }
    return true;
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