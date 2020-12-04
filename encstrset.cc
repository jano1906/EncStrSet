#include "encstrset.h"
#include <unordered_map>
#include <unordered_set>
#include <cstring>
#include <iomanip>

///LOGGER
#define LOG(msg) {if (!ndebug()) {msg; std::cerr<<std::endl;}}
#define LOG_ENTRY(msg) {if (!ndebug()){std::cerr<<__FUNCTION__<<"("; msg; std::cerr<<")"<<std::endl;}}
#define LOG_ACTION(msg) {if (!ndebug()){std::cerr<<__FUNCTION__<<": "; msg; std::cerr<<std::endl;}}


///anonymous namespace for globals and usings
namespace {
    using Id = unsigned long;
    using EncStr = std::string;
    using EncStrSet = std::unordered_set<EncStr>;
    using EncStrSetMap = std::unordered_map<Id, EncStrSet>;

    EncStrSetMap &encStrSetMap() {
        static EncStrSetMap encStrSetMap;
        return encStrSetMap;
    }

    Id &lastIdUsed() {
        static Id lastIdUsed = 0;
        return lastIdUsed;
    }

    bool &ndebug() {
#ifdef NDEBUG
        static bool ndebug = true;
#else
        static bool ndebug = false;
#endif
        return ndebug;
    }
}

///anonymous namespace for messages being logged
namespace {
    enum InsertResult {
        Inserted, AlreadyPresent_I
    };
    enum CopyResult {
        Copied, AlreadyPresent_C
    };
    enum RemoveResult {
        Removed, NotPresent_R
    };
    enum TestResult {
        Present, NotPresent_T
    };

    /*
     * Compiler accuses this function of being unused.
     * However that's false, this function is used while
     * logging message::insertLog()
     * We could do [[maybe_unused]] to make compiler happy
     * but in this case we would have to lie
     */
    std::string entry_format(const Id &x) {
        return std::to_string(x);
    }

    std::string entry_format(const char *str) {
        if (str == nullptr)
            return "NULL";
        return "\"" + std::string(str) + "\"";
    }

    namespace messages {
        void set_n(const Id &id) {
            std::cerr << "set #" << id;
        }

        void cypher(const EncStr &encStr) {
            std::cerr << "cypher " << "\"";
            char delim = ' ';
            bool isFirst = true;
            for (char c : encStr) {
                if (!isFirst)
                    std::cerr << delim;
                else
                    isFirst = false;

                std::cerr << std::uppercase
                          << std::setfill('0')
                          << std::setw(2)
                          << std::hex
                          << ((unsigned) 0xff & (unsigned int) c);
            }
            std::cerr << "\"" << std::dec;
        }

        void setAndCypher(const Id &id, const EncStr &encStr) {
            set_n(id);
            std::cerr << ", ";
            cypher(encStr);
        }

        void fName(const std::string &fName) {
            std::cerr << fName << ": ";
        }

        template<typename T, typename ... Args>
        void entryLog(T first, Args... args) {
            std::cerr << entry_format(first);
            ((std::cerr << ", " << entry_format(args)), ...);
        }

        void setNExLog(const Id &id) {
            set_n(id);
            std::cerr << " does not exist";
        }

        void invalidValueLog(const char *value) {
            std::cerr << "invalid value (" << entry_format(value) << ")";
        }

        void newLog(const Id &id) {
            set_n(id);
            std::cerr << " created";
        }

        void insertLog(const Id &id, const EncStr &encStr, InsertResult insertResult) {
            setAndCypher(id, encStr);
            switch (insertResult) {
                case Inserted:
                    std::cerr << " inserted";
                    break;
                case AlreadyPresent_I:
                    std::cerr << " was already present";
                    break;
            }
        }

        void copyLog(const EncStr &encStr, const Id &src,
                     const Id &dst, CopyResult copyResult) {
            switch (copyResult) {
                case Copied:
                    cypher(encStr);
                    std::cerr << " copied from ";
                    set_n(src);
                    std::cerr << " to ";
                    set_n(dst);
                    break;
                case AlreadyPresent_C:
                    std::cerr << "copied ";
                    cypher(encStr);
                    std::cerr << " was already present in ";
                    set_n(dst);
                    break;
            }
        }

        void removeLog(const Id &id, const EncStr &encStr,
                       RemoveResult removeResult) {
            setAndCypher(id, encStr);
            switch (removeResult) {
                case Removed:
                    std::cerr << " removed";
                    break;
                case NotPresent_R:
                    std::cerr << " was not present";
                    break;
            }
        }

        void testLog(const Id &id, const EncStr &encStr,
                     TestResult testResult) {
            setAndCypher(id, encStr);
            switch (testResult) {
                case Present:
                    std::cerr << " is present";
                    break;
                case NotPresent_T:
                    std::cerr << " is not present";
                    break;
            }
        }

        void deleteLog(const Id &id) {
            set_n(id);
            std::cerr << " deleted";
        }

        void sizeLog(const Id &id, const size_t &size) {
            set_n(id);
            std::cerr << " contains " << size << " element(s)";
        }

        void clearLog(const Id &id) {
            set_n(id);
            std::cerr << " cleared";
        }
    }
}


///anonymous namespace for helper functions
namespace {
    Id getNewId() {
        while (encStrSetMap().count(lastIdUsed()) != 0)
            lastIdUsed()++;
        return lastIdUsed();
    }

    bool invalidValue(const char *value) {
        return value == nullptr;
    }

    bool noKey(const char *key) {
        return (key == nullptr) || (strlen(key) == 0);
    }

    EncStr encrypt(const char *val, const char *key) {
        EncStr encStr;
        if (noKey(key)) {
            encStr = val;
            return encStr;
        }
        size_t vlen = strlen(val);
        size_t klen = strlen(key);
        for (size_t vit = 0, kit = 0; vit < vlen; vit++, kit = (kit + 1) % klen)
            encStr.push_back(((unsigned) val[vit]) ^ ((unsigned) key[kit]));

        return encStr;
    }

    bool paramCheck(const std::string &fName, const Id &id) {
        if (encStrSetMap().count(id) == 0) {
            LOG(messages::fName(fName);
                        messages::setNExLog(id))
            return false;
        }
        return true;
    }

    bool paramCheck(const std::string &fName, const Id &id, const char *val) {
        if (invalidValue(val)) {
            LOG(messages::fName(fName);
                        messages::invalidValueLog(val))
            return false;
        }
        return paramCheck(fName, id);
    }

    bool paramCheck(const std::string &fName, const Id &src_id, const Id &dst_id) {
        if (encStrSetMap().count(src_id) == 0) {
            LOG(messages::fName(fName);
                        messages::setNExLog(src_id))
            return false;
        }
        if (encStrSetMap().count(dst_id) == 0) {
            LOG(messages::fName(fName);
                        messages::setNExLog(dst_id))
            return false;
        }
        return true;
    }
}

///MODULE FUNCTIONS
namespace jnp1 {
/*
 * Tworzy nowy zbiór i zwraca jego identyfikator.
 */
    Id encstrset_new() {
        LOG_ENTRY()
        EncStrSet encStrSet;
        Id id = getNewId();
        encStrSetMap().insert({id, encStrSet});
        LOG_ACTION(messages::newLog(id))
        return id;
    }

/*
 * Jeżeli istnieje zbiór o identyfikatorze id, usuwa go, a w przeciwnym
 * przypadku nie robi nic.
 */
    void encstrset_delete(unsigned long id) {
        LOG_ENTRY(messages::entryLog(id))
        if (!paramCheck(__func__, id))
            return;

        encStrSetMap().erase(id);
        LOG_ACTION(messages::deleteLog(id))
    }


/*
 * Jeżeli istnieje zbiór o identyfikatorze id, zwraca liczbę jego elementów,
 * a w przeciwnym przypadku zwraca 0.
 */
    size_t encstrset_size(unsigned long id) {
        LOG_ENTRY(messages::entryLog(id))
        if (!paramCheck(__func__, id))
            return 0;

        size_t size = encStrSetMap()[id].size();
        LOG_ACTION(messages::sizeLog(id, size))
        return size;
    }

/*
 * Jeżeli istnieje zbiór o identyfikatorze id i element value po
 * zaszyfrowaniu kluczem key nie należy do tego zbioru, to dodaje ten
 * zaszyfrowany element do zbioru, a w przeciwnym przypadku nie robi nic.
 * Szyfrowanie jest symetryczne, za pomocą operacji bitowej XOR. Gdy klucz
 * key jest krótszy od value, to należy go cyklicznie powtórzyć. Wynikiem
 * jest true, gdy element został dodany, a false w przeciwnym przypadku.
 */
    bool encstrset_insert(unsigned long id, const char *value, const char *key) {
        LOG_ENTRY(messages::entryLog(id, value, key))
        if (!paramCheck(__func__, id, value))
            return false;

        EncStr encStr = encrypt(value, key);
        bool wasInserted = encStrSetMap()[id].insert(encStr).second;
        LOG_ACTION(messages::insertLog(id, encStr,
                                       wasInserted ? Inserted : AlreadyPresent_I))
        return wasInserted;
    }

/*
 * Jeżeli istnieje zbiór o identyfikatorze id i element value zaszyfrowany
 * kluczem key należy do tego zbioru, to zwraca true, a w przeciwnym
 * przypadku zwraca false.
 */
    bool encstrset_test(unsigned long id, const char *value, const char *key) {
        LOG_ENTRY(messages::entryLog(id, value, key))
        if (!paramCheck(__func__, id, value))
            return false;

        EncStr encStr = encrypt(value, key);
        bool wasFound = encStrSetMap()[id].count(encStr) != 0;
        LOG_ACTION(messages::testLog(id, encStr,
                                     wasFound ? Present : NotPresent_T))
        return wasFound;
    }

/*
 * Jeżeli istnieje zbiór o identyfikatorze id i element value zaszyfrowany
 * kluczem key należy do tego zbioru, to usuwa element ze zbioru, a w
 * przeciwnym przypadku nie robi nic. Wynikiem jest true, gdy element został
 * usunięty, a false w przeciwnym przypadku.
 */
    bool encstrset_remove(unsigned long id, const char *value, const char *key) {
        LOG_ENTRY(messages::entryLog(id, value, key))
        if (!paramCheck(__func__, id, value))
            return false;

        EncStr encStr = encrypt(value, key);
        bool wasErased = encStrSetMap()[id].erase(encStr) > 0;
        LOG_ACTION(messages::removeLog(id, encStr,
                                       wasErased ? Removed : NotPresent_R))
        return wasErased;
    }

/*
 * Jeżeli istnieje zbiór o identyfikatorze id, usuwa wszystkie jego elementy,
 * a w przeciwnym przypadku nie robi nic.
 */
    void encstrset_clear(unsigned long id) {
        LOG_ENTRY(messages::entryLog(id))
        if (!paramCheck(__func__, id))
            return;

        encStrSetMap()[id].clear();
        LOG_ACTION(messages::clearLog(id))
    }

/*
 * Jeżeli istnieją zbiory o identyfikatorach src_id oraz dst_id, to kopiuje
 * zawartość zbioru o identyfikatorze src_id do zbioru o identyfikatorze
 * dst_id, a w przeciwnym przypadku nic nie robi.
 */
    void encstrset_copy(unsigned long src_id, unsigned long dst_id) {
        LOG_ENTRY(messages::entryLog(src_id, dst_id))
        if (!paramCheck(__func__, src_id, dst_id))
            return;

        for (const EncStr &encStr : encStrSetMap()[src_id]) {
            bool wasCopied = encStrSetMap()[dst_id].insert(encStr).second;
            LOG_ACTION(messages::copyLog(encStr, src_id, dst_id,
                                         wasCopied ? Copied : AlreadyPresent_C))
        }
    }
}
