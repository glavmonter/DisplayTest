#[=======================================================================[.rst:
generate_sensors
---------------

Генерирует хедеры с описанием таблиц перевода сопротивления терморезисторов в температуру.

Аргументы
^^^^^^^^^

Зависимости
^^^^^^^^^^^

Виртуальное окружение Python3, через макрос add_python_venv

Результат работы
^^^^^^^^^^^^^^^^

Функция создаёт цели для генерации хедеров с таблицами и устанавливает следующие переменные:

- ``SENSORS_SRC`` - полный путь сгенерированного хедера

Применение
^^^^^^^^^^

Пример использования:

``add_python(${CMAKE_SOURCE_DIR}/path_to/requirements.txt)
generate_sensors()
add_executable(${TARGET_NAME} ${SENSORS_SRC})
``
#]=======================================================================]
function(generate_sensors)
    if(NOT Python3_venv_FOUND)
        message(FATAL_ERROR "The Python3 virtual environment required for `generate_sensors` function. Use `add_python_venv` macro first")
    endif()

    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/)
    include_directories(${CMAKE_CURRENT_BINARY_DIR}/include/)
    add_custom_command(
        OUTPUT include/sensors.hpp
        COMMAND ${PYTHON_VENV_EXECUTABLE} -m cogapp ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../sensors_table.hpp.cog > ${CMAKE_CURRENT_BINARY_DIR}/include/sensors.hpp
        DEPENDS ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../sensors_table.hpp.cog
        DEPENDS ${PYTHON_VENV_TARGET}
    )

    set(SENSORS_SRC
        ${CMAKE_CURRENT_BINARY_DIR}/include/sensors.hpp
        PARENT_SCOPE
    )
endfunction(generate_sensors)
