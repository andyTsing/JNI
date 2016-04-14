//
//  values.hpp
//  LibJNI
//
//  Created by Cesar Valdez on 06/04/2016.
//  Copyright © 2016 Cesar Valdez. All rights reserved.
//

#ifndef values_hpp
#define values_hpp

#include "jvm_global.hpp"
#include "jvm_handler.h"
#include "utils.h"

class JavaMethod;
class ArgumentTypeInfo;
class Arguments;


/*
 * Value : BaseValue
 *
 * Class in charge of handling the differents type between JAVA and C++, to implement a new type just need
 *  to make a template-specialization of the type you want to handle.
 *
 */

namespace LibJNI {
    
    class BaseJavaValue {
    public:
        // Override with the value expected by JNI for java Argument.
        virtual std::string GetType() { throw VMError{"Type not implemented yet."}; return ""; };
        
        //Override with the handling behavior for Native to JNI type.
        virtual jvalue GetJavaValue(JEnv& env) { throw VMError{"GetJavaValue not implemented yet for this type."}; };
        
        /*
         void SetJavaValue(JEnv& env,  jobject object) { throw VMError{"SetJavaValue not implemented yet for this type."}; }
         
         virtual std::string GetString() { throw VMError{"GetString [string], member not supported."}; }
         
         virtual int GetInt() { throw VMError{"GetInt [int], member not supported."}; }
         */
    };
    
    template <typename T>
    class Value: public BaseJavaValue {
    private:
        T value;
    public:
        //Value(T _val): value(_val) {};
        
        typedef jobject JType;
        
        void Set(JEnv& env, T _val) { value = _val; };
        T Get() {
            return value;
        }
    };
    
    
    template<>
    class Value<std::string>: public BaseJavaValue {
        std::string value;
        
    public:
        Value(std::string _value):
        value(_value){};
        Value(){};
        
        typedef jobject JType;
        
        std::string GetType() {
            return "java.lang.String";
        };
        
        jvalue GetJavaValue(JEnv& env)  {
            jvalue javaValue;
            
            javaValue.l =  env->NewStringUTF( value.c_str() );
            return javaValue;
        };
        
        std::string Get() { return value; };
        
        void Set(std::string _val) { value = _val; };
        
        void Set(JEnv& env, jobject object) {
            jstring tmp = (jstring) object;
            
            if(tmp == nullptr) value =  "Undefined";
            
            const char *str =env->GetStringUTFChars( (jstring)tmp , NULL );
            value = str;
            env->ReleaseStringUTFChars( (jstring)tmp ,str );
        };
    };
    
    
    template<>
    class Value<int>: public BaseJavaValue {
        
    private:
        int value;
        
    public:
        Value(int _value):
        value(_value){};
        Value(){};
        typedef jint JType;
        
        void Set(int _val) { value = _val; };
        void Set(JEnv& env,  jint object) { value = (int)object; }
        
        std::string GetType() { return "int"; };
        
        int Get() { return value; };
        
        jvalue GetJavaValue(JEnv& loader)  {
            jvalue javaValue;
            javaValue.i =  value;
            return javaValue;
        };
    };
    
    
    template<>
    class Value<float>: public BaseJavaValue {
        
    private:
        float value;
        
    public:
        Value(float _value):
        value(_value){};
        
        void Set(float _val) { value = _val; };
        void Set(JEnv& env,  jfloat object) { value = (jfloat)object; }
        
        std::string GetType() { return "float"; };
        
        float Get() { return value; };
        
        jvalue GetJavaValue(JEnv& loader)  {
            jvalue javaValue;
            javaValue.i =  value;
            return javaValue;
        };
    };
}







class ArgumentTypeInfo {
public:
    void Set(std::vector<std::string> arguments);
    std::string operator[](int index) { return listTypes[index]; };
    unsigned long GetNumberOfArguments(){ return parametersNumber; };
    
private:
    std::vector<std::string> listTypes;
    unsigned long parametersNumber = 0;
};



class Arguments {
    
private:
    std::vector<LibJNI::BaseJavaValue *> arguments;
    
public:
    
    static std::vector<jvalue>
    Create( JEnv jenv ,
           ArgumentTypeInfo argumentsInfo ,
           std::vector<LibJNI::BaseJavaValue *> arguments );
};


class JavaMethod: HandleEnv {
private:
    std::string name;
    std::string returnTypeInfo;
    jmethodID javaMethodRef;
    ArgumentTypeInfo argsType;
    
public:
    
    void SetName(std::string _name){ name = _name; };
    std::string GetName(){ return name; };
    
    void SetReturnTypeInfo(std::string _returnTypeInfo){ returnTypeInfo = _returnTypeInfo; };
    std::string GetReturnTypeInfo(){ return returnTypeInfo; };
    
    void SetMethodByReference(jmethodID& methodId){ javaMethodRef = methodId; };
    jmethodID GetMethodRef() { return javaMethodRef; };
    
    ArgumentTypeInfo& ArgumentsType(){ return argsType; };
    
    JavaMethod(JVMLoader loader): HandleEnv(loader) {};
};




#endif /* values_hpp */
