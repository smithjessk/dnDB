#ifndef table_exception_h
#define table_exception_h

class TableException {
    private:
        std::string error;
        TableException() {};    //not meant to be called
    public:
        TableException(std::string strError);
        std::string getError();
};

TableException::TableException(std::string strError) {
    error = strError;
}

std::string TableException::getError() {
    return error;
}

#endif
