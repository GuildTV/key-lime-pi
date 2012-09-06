#ifndef JSONUTIL_H
#define JSONUTIL_H

#ifdef DUMPJSON
#ifndef JSONDIR
#define JSONDIR "json-dump/"
#endif
#endif

#include <jsoncpp/json/json.h>
#include <vector>
#include <string>
#include <time.h>

#include "logger.h"

class JsonUtil
{
    public:
        static Json::Value VectorToJSON(std::vector<std::string> vec);
        static std::vector<std::string> JSONToVector(Json::Value data);
        static bool parseJSON(std::string *msg, Json::Value *root);
    protected:
    private:
};

#endif // JSONUTIL_H
