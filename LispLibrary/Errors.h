#pragma once
namespace errors {
    struct my_error {};

    struct lexical_error : my_error {};
    struct eos : my_error {};
    struct syntax_error : my_error {};

    struct read_final_overflow : my_error {};
}
