#pragma once

namespace floatswitch {

/**
 * @file i_binary_input.hpp
 * @brief Interface for a stable binary signal source.
 */

/**
 * @interface IBinaryInput
 * @brief Provides a debounced or filtered boolean state.
 */
class IBinaryInput
{
public:
    virtual ~IBinaryInput() = default;

    /**
     * @brief Checks if the input is currently active.
     * @return true if the signal is stable and active, false otherwise.
     */
    virtual bool is_active() = 0;
};

} // namespace floatswitch
