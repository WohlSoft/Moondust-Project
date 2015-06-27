#ifndef SMBX64_MACRO_H
#define SMBX64_MACRO_H

#define nextLine() str_count++;line = in.readLine();
#define parseLine(validate, target, converted) if( validate ) \
                                                goto badfile;\
                                                 else target=converted;

//ValueTypes
#define strVar(target, line) parseLine( SMBX64::qStr(line), target, SMBX64::StrToStr(line))
#define UIntVar(target, line) parseLine( SMBX64::uInt(line), target, line.toInt())
#define SIntVar(target, line) parseLine( SMBX64::sInt(line), target, line.toInt())
#define wBoolVar(target, line) parseLine( SMBX64::wBool(line), target, SMBX64::wBoolR(line))
#define SFltVar(target, line) parseLine( SMBX64::sFloat(line), target, line.replace(QChar(','), QChar('.')).toFloat());

#define strVarMultiLine(target, line) {\
bool first=true;\
while( (first && (line.size()==1)&&(line=="\""))||(!line.endsWith('\"')))\
{\
    first=false;\
    line.append('\n');\
    str_count++;line.append(in.readLine());\
    if(line.endsWith('\"'))\
        break;\
}\
strVar(target, line);\
}

//Version comparison
#define ge(v) file_format>=v
#define gt(v) file_format>v
#define le(v) file_format<=v
#define lt(v) file_format<v

#endif // SMBX64_MACRO_H

