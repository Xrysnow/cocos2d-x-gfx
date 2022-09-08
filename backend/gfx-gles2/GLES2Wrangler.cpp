/****************************************************************************
 Copyright (c) 2019-2022 Xiamen Yaji Software Co., Ltd.

 http://www.cocos.com

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
****************************************************************************/

#include "GLES2Wrangler.h"

#if defined(_WIN32) && !defined(ANDROID)
    #define WIN32_LEAN_AND_MEAN 1
    #include <windows.h>

static HMODULE libegl = NULL;
static HMODULE libgles = NULL;

bool gles2wOpen() {
    // libegl = LoadLibraryA("libEGL.dll");
    // libgles = LoadLibraryA("libGLESv2.dll");
    // return (libegl && libgles);
    return true;
}

bool gles2wClose() {
    bool ret = true;
    if (libegl) {
        ret &= FreeLibrary(libegl) ? true : false;
        libegl = NULL;
    }

    if (libgles) {
        ret &= FreeLibrary(libgles) ? true : false;
        libgles = NULL;
    }

    return ret;
}

void *gles2wLoad(const char *proc) {
    void *res = nullptr;
    if (eglGetProcAddress) res = (void *)eglGetProcAddress(proc);
    // if (!res) res = (void *)GetProcAddress(libegl, proc);
    return res;
}

#elif defined(__EMSCRIPTEN__)

bool gles2wOpen() { return true; }
bool gles2wClose() { return true; }
void *gles2wLoad(const char *proc) {
    return (void *)eglGetProcAddress(proc);
}

#else

#include <dlfcn.h>

static void *libegl = nullptr;
static void *libgles = nullptr;

bool gles2wOpen() {
    libegl = dlopen("libEGL.so", RTLD_LAZY | RTLD_GLOBAL);
    #if __OHOS__
    libgles = dlopen("libGLESv3.so", RTLD_LAZY | RTLD_GLOBAL);
    #else
    libgles = dlopen("libGLESv2.so", RTLD_LAZY | RTLD_GLOBAL);
    #endif
    return (libegl && libgles);
}

bool gles2wClose() {
    bool ret = true;
    if (libegl) {
        ret &= dlclose(libegl) == 0;
        libegl = nullptr;
    }

    if (libgles) {
        ret &= dlclose(libgles) == 0;
        libgles = nullptr;
    }

    return ret;
}

void *gles2wLoad(const char *proc) {
    void *res = nullptr;
    if (eglGetProcAddress) res = reinterpret_cast<void *>(eglGetProcAddress(proc));
    if (!res) res = dlsym(libegl, proc);
    return res;
}
#endif

bool gles2wInit(void) {
    // if (!gles2wOpen()) {
    //     return false;
    // }

    // eglwLoadProcs(gles2wLoad);
    // gles2wLoadProcs(gles2wLoad);

    // gladLoadEGL(nullptr, (GLADloadfunc)gles2wLoad);
    // gladLoadGLES2((GLADloadfunc)gles2wLoad);
    // gladLoaderLoadEGL(nullptr);
    // gladLoaderLoadGLES2();
    return true;
    // return gladLoaderLoadEGL(nullptr) && gladLoaderLoadGLES2();
}

bool gles2wExit() {
    return gles2wClose();
}
