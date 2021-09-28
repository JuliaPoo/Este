#pragma once

#include <pin.H>
#include <string>
#include <unordered_set>

#define TOKEN unsigned int

namespace Sync {

    class TimedMutex {

    public:

        // Constructor. Timeout in seconds. If timeout=0, there is no timeout.
        TimedMutex(uint timeout = 5);

        // Destructor
        ~TimedMutex();

        // Lock mutex. Returns true if successful. False if timeout reached
        bool lock();

        // Unlock
        void unlock();

    private:

        PIN_MUTEX mutex;

        int timeout = 0;

    };

    // Class that implements readers-writers solution. Prioritises readers.
    class RW {

    public:

        // Constructor. `is_enable` determines whether synchronisations are enabled.
        RW(void);

        // Destructor
        ~RW(void);

        // Aquire permission to write. Returns a non-zero token if successful.
        TOKEN writer_aquire(void);

        // Aquire permission to read. Returns a non-zero token if successful.
        TOKEN reader_aquire(void);

        // Release permission to write. Requires TOKEN returned from `writer_aquire` call as argument. Returns true if successful.
        bool writer_release(TOKEN owner);

        // Release permission to read. Requires TOKEN returned from `reader_aquire` call as argument. Returns true if successful.
        bool reader_release(TOKEN owner);

        // String representation
        virtual const std::string toStr(void) const;

    private:

        // Timed mutex that controls access to write
        TimedMutex mutex_rsrc;

        // Timed mutex that controls access to variables keeping track of reads
        TimedMutex mutex_read;

        // Tracks number of readers.
        unsigned int _read_count = 0;

        // Write owner token
        TOKEN wowner = 0;

        // Read owner token
        std::tr1::unordered_set<TOKEN> rowner = std::tr1::unordered_set<TOKEN>({});

        // Aquire permission to write. Returns true if successful
        TOKEN _writer_aquire(void);

        // Aquire permission to read. Returns true if successful
        TOKEN _reader_aquire(void);

        // Release permission to write. Returns true if successful
        bool _writer_release(TOKEN owner);

        // Release permission to read. Returns true if successful
        bool _reader_release(TOKEN owner);
    };
}