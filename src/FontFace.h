#ifndef FONTFACE_H
#define FONTFACE_H

#include <ft2build.h>
#include FT_GLYPH_H

#include <v8.h>
#include <node.h>
#include <nan.h>

class FontFace : public Nan::ObjectWrap {
    public: 
		static void Init();   
		static v8::Local<v8::FunctionTemplate> CreateConstructorTemplate();
        static v8::Local<v8::Object> NewInstance(v8::Local<v8::Value> name);
    	static v8::Local<v8::Function> GetConstructor();
		FT_Face face;
	private:
		FontFace(uint8_t dpi);
		~FontFace();
		uint8_t dpi;
		
		static Nan::Persistent<v8::Function> constructor;
		static void New(const Nan::FunctionCallbackInfo<v8::Value>& info);
		static NAN_GETTER(max_advance_width);
		static NAN_GETTER(GetSize);
		static NAN_SETTER(SetSize);
		static NAN_GETTER(bbox);
		static NAN_GETTER(unitsPerEm);
		static NAN_GETTER(GetHeight);
		static NAN_SETTER(SetHeight);
};

#endif