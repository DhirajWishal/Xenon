#if 0
; SPIR-V
; Version: 1.6
; Generator: Google spiregg; 0
; Bound: 23
; Schema: 0
               OpCapability Shader
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %main "main" %in_var_TEXCOORD0 %in_var_COLOR0 %out_var_SV_TARGET %baseColorTexture
               OpExecutionMode %main OriginUpperLeft
               OpSource HLSL 660
               OpName %type_2d_image "type.2d.image"
               OpName %baseColorTexture "baseColorTexture"
               OpName %in_var_TEXCOORD0 "in.var.TEXCOORD0"
               OpName %in_var_COLOR0 "in.var.COLOR0"
               OpName %out_var_SV_TARGET "out.var.SV_TARGET"
               OpName %main "main"
               OpName %type_sampled_image "type.sampled.image"
               OpDecorate %in_var_TEXCOORD0 Location 0
               OpDecorate %in_var_COLOR0 Location 1
               OpDecorate %out_var_SV_TARGET Location 0
               OpDecorate %baseColorTexture DescriptorSet 1
               OpDecorate %baseColorTexture Binding 0
      %float = OpTypeFloat 32
%type_2d_image = OpTypeImage %float 2D 2 0 0 1 Unknown
    %v4float = OpTypeVector %float 4
%_ptr_Input_v4float = OpTypePointer Input %v4float
    %v2float = OpTypeVector %float 2
%_ptr_Input_v2float = OpTypePointer Input %v2float
%_ptr_Output_v4float = OpTypePointer Output %v4float
       %void = OpTypeVoid
         %15 = OpTypeFunction %void
%type_sampled_image = OpTypeSampledImage %type_2d_image
%in_var_TEXCOORD0 = OpVariable %_ptr_Input_v2float Input
%in_var_COLOR0 = OpVariable %_ptr_Input_v4float Input
%out_var_SV_TARGET = OpVariable %_ptr_Output_v4float Output
%_ptr_UniformConstant_type_sampled_image = OpTypePointer UniformConstant %type_sampled_image
%baseColorTexture = OpVariable %_ptr_UniformConstant_type_sampled_image UniformConstant
       %main = OpFunction %void None %15
         %17 = OpLabel
         %18 = OpLoad %v2float %in_var_TEXCOORD0
         %19 = OpLoad %v4float %in_var_COLOR0
         %20 = OpLoad %type_sampled_image %baseColorTexture
         %21 = OpImageSampleImplicitLod %v4float %20 %18 None
         %22 = OpFMul %v4float %19 %21
               OpStore %out_var_SV_TARGET %22
               OpReturn
               OpFunctionEnd

#endif

const unsigned char g_IMGUILAYER_FRAG_SPIRV[] = {
  0x03, 0x02, 0x23, 0x07, 0x00, 0x06, 0x01, 0x00, 0x00, 0x00, 0x0e, 0x00,
  0x17, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x02, 0x00,
  0x01, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x01, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x09, 0x00, 0x04, 0x00, 0x00, 0x00,
  0x01, 0x00, 0x00, 0x00, 0x6d, 0x61, 0x69, 0x6e, 0x00, 0x00, 0x00, 0x00,
  0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
  0x05, 0x00, 0x00, 0x00, 0x10, 0x00, 0x03, 0x00, 0x01, 0x00, 0x00, 0x00,
  0x07, 0x00, 0x00, 0x00, 0x03, 0x00, 0x03, 0x00, 0x05, 0x00, 0x00, 0x00,
  0x94, 0x02, 0x00, 0x00, 0x05, 0x00, 0x06, 0x00, 0x06, 0x00, 0x00, 0x00,
  0x74, 0x79, 0x70, 0x65, 0x2e, 0x32, 0x64, 0x2e, 0x69, 0x6d, 0x61, 0x67,
  0x65, 0x00, 0x00, 0x00, 0x05, 0x00, 0x07, 0x00, 0x05, 0x00, 0x00, 0x00,
  0x62, 0x61, 0x73, 0x65, 0x43, 0x6f, 0x6c, 0x6f, 0x72, 0x54, 0x65, 0x78,
  0x74, 0x75, 0x72, 0x65, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x07, 0x00,
  0x02, 0x00, 0x00, 0x00, 0x69, 0x6e, 0x2e, 0x76, 0x61, 0x72, 0x2e, 0x54,
  0x45, 0x58, 0x43, 0x4f, 0x4f, 0x52, 0x44, 0x30, 0x00, 0x00, 0x00, 0x00,
  0x05, 0x00, 0x06, 0x00, 0x03, 0x00, 0x00, 0x00, 0x69, 0x6e, 0x2e, 0x76,
  0x61, 0x72, 0x2e, 0x43, 0x4f, 0x4c, 0x4f, 0x52, 0x30, 0x00, 0x00, 0x00,
  0x05, 0x00, 0x07, 0x00, 0x04, 0x00, 0x00, 0x00, 0x6f, 0x75, 0x74, 0x2e,
  0x76, 0x61, 0x72, 0x2e, 0x53, 0x56, 0x5f, 0x54, 0x41, 0x52, 0x47, 0x45,
  0x54, 0x00, 0x00, 0x00, 0x05, 0x00, 0x04, 0x00, 0x01, 0x00, 0x00, 0x00,
  0x6d, 0x61, 0x69, 0x6e, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x07, 0x00,
  0x07, 0x00, 0x00, 0x00, 0x74, 0x79, 0x70, 0x65, 0x2e, 0x73, 0x61, 0x6d,
  0x70, 0x6c, 0x65, 0x64, 0x2e, 0x69, 0x6d, 0x61, 0x67, 0x65, 0x00, 0x00,
  0x47, 0x00, 0x04, 0x00, 0x02, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00, 0x03, 0x00, 0x00, 0x00,
  0x1e, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00,
  0x04, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x47, 0x00, 0x04, 0x00, 0x05, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, 0x00,
  0x01, 0x00, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00, 0x05, 0x00, 0x00, 0x00,
  0x21, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16, 0x00, 0x03, 0x00,
  0x08, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x19, 0x00, 0x09, 0x00,
  0x06, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
  0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x17, 0x00, 0x04, 0x00,
  0x09, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
  0x20, 0x00, 0x04, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
  0x09, 0x00, 0x00, 0x00, 0x17, 0x00, 0x04, 0x00, 0x0b, 0x00, 0x00, 0x00,
  0x08, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x00,
  0x0c, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x00, 0x00,
  0x20, 0x00, 0x04, 0x00, 0x0d, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
  0x09, 0x00, 0x00, 0x00, 0x13, 0x00, 0x02, 0x00, 0x0e, 0x00, 0x00, 0x00,
  0x21, 0x00, 0x03, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x00,
  0x1b, 0x00, 0x03, 0x00, 0x07, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00,
  0x3b, 0x00, 0x04, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
  0x01, 0x00, 0x00, 0x00, 0x3b, 0x00, 0x04, 0x00, 0x0a, 0x00, 0x00, 0x00,
  0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x3b, 0x00, 0x04, 0x00,
  0x0d, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
  0x20, 0x00, 0x04, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x07, 0x00, 0x00, 0x00, 0x3b, 0x00, 0x04, 0x00, 0x10, 0x00, 0x00, 0x00,
  0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x05, 0x00,
  0x0e, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x0f, 0x00, 0x00, 0x00, 0xf8, 0x00, 0x02, 0x00, 0x11, 0x00, 0x00, 0x00,
  0x3d, 0x00, 0x04, 0x00, 0x0b, 0x00, 0x00, 0x00, 0x12, 0x00, 0x00, 0x00,
  0x02, 0x00, 0x00, 0x00, 0x3d, 0x00, 0x04, 0x00, 0x09, 0x00, 0x00, 0x00,
  0x13, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x3d, 0x00, 0x04, 0x00,
  0x07, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00,
  0x57, 0x00, 0x06, 0x00, 0x09, 0x00, 0x00, 0x00, 0x15, 0x00, 0x00, 0x00,
  0x14, 0x00, 0x00, 0x00, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x85, 0x00, 0x05, 0x00, 0x09, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00,
  0x13, 0x00, 0x00, 0x00, 0x15, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x03, 0x00,
  0x04, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0xfd, 0x00, 0x01, 0x00,
  0x38, 0x00, 0x01, 0x00
};