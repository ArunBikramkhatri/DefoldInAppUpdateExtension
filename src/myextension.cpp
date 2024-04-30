// myextension.cpp
// Extension lib defines
#define EXTENSION_NAME myExtension
#define LIB_NAME "myExtension"
#define MODULE_NAME LIB_NAME

// include the Defold SDK
#include <dmsdk/sdk.h>
// #include <native_activity.h>

#if defined(DM_PLATFORM_ANDROID)

static JNIEnv* Attach()
{
    JNIEnv* env;
    JavaVM* vm = dmGraphics::GetNativeAndroidJavaVM();
    vm->AttachCurrentThread(&env, NULL);
    return env;
}

static bool Detach(JNIEnv* env)
{
    bool exception = (bool) env->ExceptionCheck();
    env->ExceptionClear();
    JavaVM* vm = dmGraphics::GetNativeAndroidJavaVM();
    vm->DetachCurrentThread();
    return !exception;
}

namespace {
struct AttachScope
{
    JNIEnv* m_Env;
    AttachScope() : m_Env(Attach())
    {
        dmLogInfo("attach scope");
    }
    ~AttachScope()
    {
        // dmLogInfo("detach scope");
        Detach(m_Env);
    }
};
}

static jclass GetClass(JNIEnv* env, const char* classname)
{
    jclass activity_class = env->FindClass("android/app/NativeActivity");
    jmethodID get_class_loader = env->GetMethodID(activity_class,"getClassLoader", "()Ljava/lang/ClassLoader;");
    jobject cls = env->CallObjectMethod(dmGraphics::GetNativeAndroidActivity(), get_class_loader);
    jclass class_loader = env->FindClass("java/lang/ClassLoader");
    jmethodID find_class = env->GetMethodID(class_loader, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");

    jstring str_class_name = env->NewStringUTF(classname);
    jclass outcls = (jclass)env->CallObjectMethod(cls, find_class, str_class_name);
    env->DeleteLocalRef(str_class_name);
    return outcls;
}
void SetActivity(JNIEnv* env , jobject Activity){
    jclass cls = GetClass(env , "com.example.admob.Admob");
    dmLogInfo("get class");
    if (cls == nullptr) {
            dmLogError("Failed to find class com.example.admob.Admob");
            // sreturn 0; // Or return an error code indicating failure
            return;
        } 
    jobject obj = env->NewObject(cls ,env->GetMethodID(cls , "<init>" ,"()V"));

    jmethodID method = env->GetMethodID(cls, "setActivity" , "(Landroid/app/Activity;)V");
    if (method == NULL)
    {
        dmLogError("Method is null");
        // Error handling
        return;
 
    }
    env->CallVoidMethod(obj, method ,Activity);
}

static int SetActivityFromDefold(lua_State* L){
    // DM_LUA_STACK_CHECK(L ,1);
    AttachScope attachScope;
    JNIEnv* env = attachScope.m_Env;
    dmLogInfo("setActivityFromDefold");
    jobject native_activity = dmGraphics::GetNativeAndroidActivity();
    // jobject activity = native_activity->clazz;

    // Call the function to set the Activity in Java
    SetActivity(env, native_activity);
    dmLogInfo("setActivity");
    return 0 ;
}

static int ShowAd(lua_State* L){
    // DM_LUA_STACK_CHECK(L ,1);
    AttachScope attachScope;
    JNIEnv* env = attachScope.m_Env;

    jclass cls = GetClass(env , "com.example.admob.Admob");
    if (cls == nullptr) {
        dmLogError("Failed to find class com.example.admob.Admob");
        return 0; // Or return an error code indicating failure
    }

    jmethodID method = env->GetStaticMethodID(cls, "showAd", "()Ljava/lang/String;");
    if (method == nullptr) {
        dmLogError("Failed to find method showAd");
        return 0; // Or return an error code indicating failure
    }

    jstring return_value = (jstring)env->CallStaticObjectMethod(cls, method);
    if (return_value == nullptr) {
        dmLogError("Method invocation failed");
        return 0; // Or return an error code indicating failure
    }

    const char* str = env->GetStringUTFChars(return_value, 0);
    lua_pushstring(L, str);
    env->ReleaseStringUTFChars(return_value, str);
    env->DeleteLocalRef(return_value);
    
    return 1;
}

static int ShowBannerAd(lua_State* L){
    // DM_LUA_STACK_CHECK(L,1);
    AttachScope attachScope;
    JNIEnv* env = attachScope.m_Env;
    jclass cls = GetClass(env , "com.example.admob.Admob");
    if (cls == nullptr) {
        dmLogError("Failed to find class com.example.admob.Admob");
        return 0; // Or return an error code indicating failure
    }
    dmLogInfo("Admob Class found");
    jmethodID method = env->GetStaticMethodID(cls, "showBannerAd", "()Ljava/lang/String;");
    if (method == nullptr) {
        dmLogError("Failed to find method showBannerAd");
        return 0; // Or return an error code indicating failure
    }
    jstring return_value = (jstring)env->CallStaticObjectMethod(cls ,method);
     if (return_value == nullptr) {
        dmLogError("Method invocation failed");
        return 0; // Or return an error code indicating failure
    }
    const char* str =env->GetStringUTFChars(return_value ,0);
    lua_pushstring(L,str);
    env->ReleaseStringUTFChars(return_value ,str);
    env->DeleteLocalRef(return_value);
    
    return 1; 

}

static int Reverse(lua_State* L)
{
    // The number of expected items to be on the Lua stack
    // once this struct goes out of scope
    DM_LUA_STACK_CHECK(L, 1);

    // Check and get parameter string from stack
    char* str = (char*)luaL_checkstring(L, 1);

    // Reverse the string
    int len = strlen(str);
    for(int i = 0; i < len / 2; i++) {
        const char a = str[i];
        const char b = str[len - i - 1];
        str[i] = b;
        str[len - i - 1] = a;
    }

    // Put the reverse string on the stack
    lua_pushstring(L, str);

    // Return 1 item
    return 1;
}

// Functions exposed to Lua
static const luaL_reg Module_methods[] =
{
    {"reverse", Reverse},
    {"show_ad" , ShowAd},
    {"set_activity_from_defold" , SetActivityFromDefold},
    {"show_banner_ad" ,ShowBannerAd},
    {0, 0}
};

static void LuaInit(lua_State* L)
{
    int top = lua_gettop(L);

    // Register lua names
    luaL_register(L, MODULE_NAME, Module_methods);

    lua_pop(L, 1);
    assert(top == lua_gettop(L));
}

static dmExtension::Result AppInitializeMyExtension(dmExtension::AppParams* params)
{
    dmLogInfo("AppInitializeMyExtension");
    return dmExtension::RESULT_OK;
}

static dmExtension::Result InitializeMyExtension(dmExtension::Params* params)
{
    // Init Lua
    LuaInit(params->m_L);
    dmLogInfo("Registered %s Extension", MODULE_NAME);
    return dmExtension::RESULT_OK;
}

static dmExtension::Result AppFinalizeMyExtension(dmExtension::AppParams* params)
{
    dmLogInfo("AppFinalizeMyExtension");
    return dmExtension::RESULT_OK;
}

static dmExtension::Result FinalizeMyExtension(dmExtension::Params* params)
{
    dmLogInfo("FinalizeMyExtension");
    return dmExtension::RESULT_OK;
}

static dmExtension::Result OnUpdateMyExtension(dmExtension::Params* params)
{
    // dmLogInfo("OnUpdateMyExtension");
    return dmExtension::RESULT_OK;
}

static void OnEventMyExtension(dmExtension::Params* params, const dmExtension::Event* event)
{
    switch(event->m_Event)
    {
        case dmExtension::EVENT_ID_ACTIVATEAPP:
        dmLogInfo("OnEventMyExtension - EVENT_ID_ACTIVATEAPP");
        break;
        case dmExtension::EVENT_ID_DEACTIVATEAPP:
        dmLogInfo("OnEventMyExtension - EVENT_ID_DEACTIVATEAPP");
        break;
        case dmExtension::EVENT_ID_ICONIFYAPP:
        dmLogInfo("OnEventMyExtension - EVENT_ID_ICONIFYAPP");
        break;
        case dmExtension::EVENT_ID_DEICONIFYAPP:
        dmLogInfo("OnEventMyExtension - EVENT_ID_DEICONIFYAPP");
        break;
        default:
        dmLogWarning("OnEventMyExtension - Unknown event id");
        break;
    }
}
#else
static dmExtension::Result AppInitializeMyExtension(dmExtension::AppParams* params)
{
    dmLogInfo("AppInitializeMyExtension");
    return dmExtension::RESULT_OK;
}

static dmExtension::Result InitializeMyExtension(dmExtension::Params* params)
{
    // Init Lua
    // LuaInit(params->m_L);
    dmLogWarning("Registered %s (null) Extension\n", MODULE_NAME);
    return dmExtension::RESULT_OK;
}
static dmExtension::Result AppFinalizeMyExtension(dmExtension::AppParams* params)
{
    dmLogInfo("AppFinalizeMyExtension");
    return dmExtension::RESULT_OK;
}

static dmExtension::Result FinalizeMyExtension(dmExtension::Params* params)
{
    dmLogInfo("FinalizeMyExtension");
    return dmExtension::RESULT_OK;
}

static dmExtension::Result OnUpdateMyExtension(dmExtension::Params* params)
{
    // dmLogInfo("OnUpdateMyExtension");
    return dmExtension::RESULT_OK;
}

static void OnEventMyExtension(dmExtension::Params* params, const dmExtension::Event* event)
{
    switch(event->m_Event)
    {
        case dmExtension::EVENT_ID_ACTIVATEAPP:
        dmLogInfo("OnEventMyExtension - EVENT_ID_ACTIVATEAPP");
        break;
        case dmExtension::EVENT_ID_DEACTIVATEAPP:
        dmLogInfo("OnEventMyExtension - EVENT_ID_DEACTIVATEAPP");
        break;
        case dmExtension::EVENT_ID_ICONIFYAPP:
        dmLogInfo("OnEventMyExtension - EVENT_ID_ICONIFYAPP");
        break;
        case dmExtension::EVENT_ID_DEICONIFYAPP:
        dmLogInfo("OnEventMyExtension - EVENT_ID_DEICONIFYAPP");
        break;
        default:
        dmLogWarning("OnEventMyExtension - Unknown event id");
        break;
    }
}

#endif

// Defold SDK uses a macro for setting up extension entry points:
//
// DM_DECLARE_EXTENSION(symbol, name, app_init, app_final, init, update, on_event, final)

// MyExtension is the C++ symbol that holds all relevant extension data.
// It must match the name field in the `ext.manifest`
DM_DECLARE_EXTENSION(EXTENSION_NAME, LIB_NAME, AppInitializeMyExtension, AppFinalizeMyExtension, InitializeMyExtension, OnUpdateMyExtension, OnEventMyExtension, FinalizeMyExtension)
