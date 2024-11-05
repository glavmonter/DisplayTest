#[=======================================================================[.rst:
generate_commands
---------------

Добавляет автогенерируемые хедеры с описанием команд управления к коду.

Аргументы
^^^^^^^^^

- ``COMMANDS_JSON`` - полный путь до файла json с описанием команд

Зависимости
^^^^^^^^^^^

Виртуальное окружение Python3, через макрос add_python_venv

Результат работы
^^^^^^^^^^^^^^^^

Функция создаёт цели для генерации хедеров с командами и устанавливает следующие переменные:

- ``COMMANDS_SRC`` - полный путь сгенерированного хедера

Применение
^^^^^^^^^^

Пример использования:

``add_python(${CMAKE_SOURCE_DIR}/path_to/requirements.txt)
generate_commands(${CMAKE_SOURCE_DIR}/common/commands.json)
add_executable(${TARGET_NAME} ${COMMANDS_SRC})
``
#]=======================================================================]
function(generate_commands COMMANDS_JSON)
    if(NOT Python3_venv_FOUND)
        message(FATAL_ERROR "The Python3 virtual environment required for `generate_commands` function. Use `add_python_venv` macro first")
    endif()

    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/)
    include_directories(${CMAKE_CURRENT_BINARY_DIR}/include/)
    add_custom_command(
        OUTPUT include/commands.hpp
        COMMAND ${PYTHON_VENV_EXECUTABLE} -m cogapp -Dcommand_json=${COMMANDS_JSON} ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../commands.hpp.cog > ${CMAKE_CURRENT_BINARY_DIR}/include/commands.hpp
        DEPENDS ${COMMANDS_JSON}
        DEPENDS ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../commands.hpp.cog
        DEPENDS ${PYTHON_VENV_TARGET}
    )

    set(COMMANDS_SRC
        ${CMAKE_CURRENT_BINARY_DIR}/include/commands.hpp
        PARENT_SCOPE
    )
endfunction(generate_commands)
