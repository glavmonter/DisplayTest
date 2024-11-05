#include <cmath>
#include "utils.hpp"

/**
 * @brief Бинарный поиск индекса в таблице, близкого к value. Таблица отсортирована по возрастанию
 * @param[in] value Искомое значение
 * @param[in] pDataTable Таблица поиска
 * @param[in] dataTableSize Размер таблицы в элементах
 * @return uint16_t индекс ближайшего к value значение
 */
uint16_t BinarySearch(float value, const float *pDataTable, size_t dataTableSize) {
    if (value < pDataTable[0]) {
        return 0;
    }
    if (value > pDataTable[dataTableSize - 1]) {
        return dataTableSize - 1;
    }

int lo = 0;
int hi = dataTableSize - 1;

    while (lo <= hi) {
        int mid = (lo + hi) / 2;
        if (value < pDataTable[mid]) {
            hi = mid - 1;
        } else if (value > pDataTable[mid]) {
            lo = mid + 1;
        } else {
            return mid;
        }
    }

    return (pDataTable[lo] - value) < (value - pDataTable[hi]) ? lo : hi;
}


/**
 * @brief Ленейная интерполяция x между двух точек (x0, y0) - (x1, y1)
 * @param[in] x0  
 * @param[in] y0 
 * @param[in] x1 
 * @param[in] y1 
 * @param[in] x Искомая ордината
 * @return float Значение Y для искомого X
 */
float LinearInterpolation(float x0, float y0, float x1, float y1, float x) {
float y = 0;
    if (::fabs(x1 - x0) > std::numeric_limits<float>::epsilon()) {
        y = y0  + (y1 - y0) * (x - x0) / (x1 - x0);
    }
    return y;
}
