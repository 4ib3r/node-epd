
#include <nan.h>
#include "FontFace.h"

using namespace v8;

Nan::Persistent<v8::Function> FontFace::constructor;

FontFace::FontFace(uint8_t dpi) {
  this->dpi = dpi; 
}

FontFace::~FontFace() {
	FT_Done_Face(this->face);
}

v8::Local<v8::FunctionTemplate> FontFace::CreateConstructorTemplate() {
  Nan::EscapableHandleScope scope;

  v8::Local<v8::FunctionTemplate> constructorTemplate = Nan::New<v8::FunctionTemplate>(New);
  constructorTemplate->SetClassName(Nan::New("FontFace").ToLocalChecked());

  v8::Local<v8::ObjectTemplate> instanceTemplate = constructorTemplate->InstanceTemplate();
  
  Nan::SetAccessor(instanceTemplate, Nan::New("size").ToLocalChecked(), GetSize, SetSize);
  Nan::SetAccessor(instanceTemplate, Nan::New("max_advance_width").ToLocalChecked(), max_advance_width);
  Nan::SetAccessor(instanceTemplate, Nan::New("units_per_EM").ToLocalChecked(), unitsPerEm);
  Nan::SetAccessor(instanceTemplate, Nan::New("height").ToLocalChecked(), GetHeight, SetHeight);
  instanceTemplate->SetInternalFieldCount(1);

  return scope.Escape(constructorTemplate);
}

v8::Local<v8::Function> FontFace::GetConstructor() {
  Nan::EscapableHandleScope scope;

  v8::Local<v8::Function> localConstructor;

  if (constructor.IsEmpty()) {
    localConstructor = Nan::GetFunction(CreateConstructorTemplate()).ToLocalChecked();
    constructor.Reset(localConstructor);
  } else {
    localConstructor = Nan::New(constructor);
  }

  return scope.Escape(localConstructor);
}

NAN_METHOD(FontFace::New) {
  uint8_t dpi = info[0]->NumberValue();
  FontFace* fontFace = new FontFace(dpi);
  fontFace->Wrap(info.This());
  info.GetReturnValue().Set(info.This());
}

NAN_SETTER(FontFace::SetSize) {
  Nan::HandleScope scope;
  FontFace *obj = Nan::ObjectWrap::Unwrap<FontFace>(info.Holder());
  uint8_t size = (value->NumberValue());
  FT_Set_Char_Size(obj->face, size << 6, 0, obj->dpi, 0);
}

NAN_GETTER(FontFace::GetSize) {
  FontFace* fontFace = Nan::ObjectWrap::Unwrap<FontFace>(info.This());
  info.GetReturnValue().Set(Nan::New((int32_t)fontFace->face->height));
}

NAN_GETTER(FontFace::max_advance_width) {
  FontFace* fontFace = Nan::ObjectWrap::Unwrap<FontFace>(info.This());
  info.GetReturnValue().Set(Nan::New((int32_t)fontFace->face->max_advance_width));
}

NAN_GETTER(FontFace::bbox) {
  FontFace* fontFace = Nan::ObjectWrap::Unwrap<FontFace>(info.This());
  v8::Local<v8::Object> bbox = Nan::New<v8::Object>();
  bbox->Set(Nan::New("xMin").ToLocalChecked(), Nan::New((int32_t)fontFace->face->bbox.xMin));
  bbox->Set(Nan::New("xMax").ToLocalChecked(), Nan::New((int32_t)fontFace->face->bbox.xMax));
  bbox->Set(Nan::New("yMin").ToLocalChecked(), Nan::New((int32_t)fontFace->face->bbox.yMin));
  bbox->Set(Nan::New("yMax").ToLocalChecked(), Nan::New((int32_t)fontFace->face->bbox.yMax));
  info.GetReturnValue().Set(bbox);
}

NAN_SETTER(FontFace::SetHeight) {
  Nan::HandleScope scope;
  FontFace *obj = Nan::ObjectWrap::Unwrap<FontFace>(info.Holder());
  uint8_t size = (value->NumberValue());
  FT_Set_Pixel_Sizes(obj->face, 0, size);
}

NAN_GETTER(FontFace::GetHeight) {
  FontFace* fontFace = Nan::ObjectWrap::Unwrap<FontFace>(info.This());
  info.GetReturnValue().Set(Nan::New((int32_t)fontFace->face->height));
}

NAN_GETTER(FontFace::unitsPerEm) {
  FontFace* fontFace = Nan::ObjectWrap::Unwrap<FontFace>(info.This());
  info.GetReturnValue().Set(Nan::New((int32_t)fontFace->face->units_per_EM));
}