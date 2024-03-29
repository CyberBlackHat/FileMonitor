#ifndef __SHANNON_ENTROPY_H_
#define __SHANNON_ENTROPY_H_
#pragma once
#include <algorithm>
#include <vector>
#include <string>
#include <map>
#include <cmath>


namespace entropy {

    /// @brief Accept range of probabilities per byte
    /// Zero-probability in the sequence could be skipped
    /// @return entropy if everything ok, -1.0 if probabilities range overflows one byte
    /// Formula is here: https://en.wiktionary.org/wiki/Shannon_entropy
    template <typename T>
    double shannon_entropy(T first, T last)
    {
        size_t frequencies_count{};
        double entropy{};

        std::for_each(first, last, [&entropy, &frequencies_count](auto item) mutable {

            if (0. == item) return;
            double fp_item = static_cast<double>(item);
            entropy += fp_item * log2(fp_item);
            ++frequencies_count;
            });

        if (frequencies_count > 256) {
            return -1.0;
        }

        return -entropy;
    }

    template <typename T, typename CallbackObj>
    double shannon_entropy(T first, T last, CallbackObj& callback)
    {
        size_t frequencies_count{};
        double entropy{};

        uintmax_t counter{};
        uintmax_t data_size = std::distance(first, last);
        callback.init(data_size);

        std::for_each(first, last, [&entropy, &frequencies_count, &callback](auto item) mutable {

            if (0. == item) return;
            double fp_item = static_cast<double>(item);
            entropy += fp_item * log2(fp_item);
            ++frequencies_count;
            });

        if (frequencies_count > 256) {
            assert(false);
            return -1.0;
        }

        return -entropy;
    }


    /// @brief Detect whether some sequence (byte, block, memory, disk) is encrypted or highly compressed
    class ShannonEncryptionChecker {
    public:

        enum InformationEntropyEstimation {
            Plain,
            Binary,
            Encrypted,
            Unknown,
            EntropyLevelSize
        };

        /// @brief Callback type for calling on all iterations
        using callback_t = void(*)(uintmax_t);

        /// @brief Set facet for unsigned char (boost binary reading twice)
        ShannonEncryptionChecker();

        /// @brief Detect whether file encrypted or very highly compressed with high enough probability
        /// @param file_path: full file path
        /// @param epsilon: estimated difference between absolute chaos (8.0) and actual entropy
        double get_file_entropy(const std::string& file_path) const;

        /// @brief Detect whether the bytes sequence (e.g. memory) is encrypted
        double get_sequence_entropy(const uint8_t* sequence_start, size_t sequence_size) const;

        /// @brief Set callback function, accepting value of the bytes counter
        void set_callback(callback_t callback);

        /// @brief Get information encryption level using provided entropy and sequence size
        InformationEntropyEstimation information_entropy_estimation(double entropy, size_t sequence_size) const;

        /// @brief Min possible file size assuming max theoretical compression efficiency in bytes
        size_t min_compressed_size(double entropy, size_t sequence_size) const;

        /// @brief Provide readable properties of the information sequence
        std::string get_information_description(InformationEntropyEstimation ent) const;

        /// @brief Interrupt all calculating threads
        static void interrupt();


    private:

        /// do not make it atomic so that avoid cache ping-pong
        static bool interrupt_all_;

        /// Callback function called on every iteration
        callback_t callback_{};

        /// Calculate probabilities to meet some byte in the file
        std::vector<double> read_file_probabilities(const std::string& file_path, size_t file_size) const;

        /// Calculate probabilities to meet some byte in the sequence
        std::vector<double> read_stream_probabilities(const uint8_t* sequence_start, size_t sequence_size) const;

        /// Relate epsilon to checked file size
        /// Entropy of encrypted file very close to 8.0 (like 7.999998..)
        /// However estimation depends on the sample size
        /// Than bigger the sample than smaller the epsilon
        double estimated_epsilon(size_t sample_size) const;

        /// Static flag, set while we load uint8_t facet for the first time
        static bool load_uint8_codecvt_;

        /// Map information properties to string description
        static std::map<InformationEntropyEstimation, std::string> entropy_string_description_;

        /// Buffer size, should not be close to 1 MB as created on a thread stack
        static constexpr size_t MAX_BUFFER_SIZE = 1024 * 64;
    };

} // namespace entropy

#endif //__SHANNON_ENTROPY_H_
