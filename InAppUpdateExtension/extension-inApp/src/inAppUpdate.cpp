// inAppUpdate.cpp
// Extension lib defines
#define EXTENSION_NAME inAppUpdate
#define LIB_NAME "inAppUpdate"
#define MODULE_NAME LIB_NAME

// include the Defold SDK
#include <dmsdk/sdk.h>
// #include <native_activity.h>

#if defined(DM_PLATFORM_ANDROID)

#include <dmsdk/dlib/android.h>

struct InAppUpdate 
{
    jobject        m_InAppUpateJNI;
    jmethodID      m_Initialize;
};

static InAppUpdate  g_InAppUpdate;

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


void in_app_initialize(jobject Activity){

    dmAndroid::ThreadAttacher threadAttacher;
    JNIEnv* env = threadAttacher.GetEnv();
    jclass cls = dmAndroid::LoadClass(env, "com.rummy.inAppUpdate.InAppUpdateJNI");

    // InitJNIMethods(env, cls);
    dmLogInfo("get class")
    jmethodID jni_constructor = env->GetMethodID(cls, "<init>", "(Landroid/app/Activity;)V");

    g_InAppUpdate.m_InAppUpateJNI = env->NewGlobalRef(env->NewObject(cls, jni_constructor, Activity));
}

static int InAppInitialize(lua_State* L){
    dmLogInfo("InAppInitialize");
    jobject native_activity = dmGraphics::GetNativeAndroidActivity();
    // Call the function to set the Activity in Java
    in_app_initialize(native_activity);
    dmLogInfo("in_app_initialize");
    return 0 ;
}



// Functions exposed to Lua
static const luaL_reg Module_methods[] =
{
    {"in_app_initialize" , InAppInitialize},
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
