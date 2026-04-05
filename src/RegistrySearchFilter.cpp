/* Copyright 2022 the SumatraPDF project authors (see AUTHORS file).
   License: GPLv3 */

#include "utils/BaseUtil.h"
#include "utils/WinUtil.h"

#include "RegistrySearchFilter.h"

#include "utils/Log.h"

bool InstallSearchFilter(const char* dllPath, bool allUsers) {
    struct {
        const char *key, *value, *data;
    } regVals[] = {
        {"Software\\Classes\\CLSID\\" kPdfFilter2Clsid, nullptr, "SumatraPDF IFilter"},
        {"Software\\Classes\\CLSID\\" kPdfFilter2Clsid "\\InProcServer32", nullptr, dllPath},
        {"Software\\Classes\\CLSID\\" kPdfFilter2Clsid "\\InProcServer32", "ThreadingModel", "Both"},
        {"Software\\Classes\\CLSID\\" kPdfFilter2Handler, nullptr, "SumatraPDF IFilter Persistent Handler"},
        {"Software\\Classes\\CLSID\\" kPdfFilter2Handler "\\PersistentAddinsRegistered", nullptr, ""},
        {"Software\\Classes\\CLSID"
         "\\" kPdfFilter2Handler "\\PersistentAddinsRegistered\\{89BCB740-6119-101A-BCB7-00DD010655AF}",
         nullptr, kPdfFilter2Clsid},
        {"Software\\Classes\\.pdf\\PersistentHandler", nullptr, kPdfFilter2Handler},
        {"Software\\Classes\\CLSID\\" kTexFilter2Clsid, nullptr, "SumatraPDF IFilter"},
        {"Software\\Classes\\CLSID\\" kTexFilter2Clsid "\\InProcServer32", nullptr, dllPath},
        {"Software\\Classes\\CLSID\\" kTexFilter2Clsid "\\InProcServer32", "ThreadingModel", "Both"},
        {"Software\\Classes\\CLSID\\" kTexFilter2Handler, nullptr, "SumatraPDF LaTeX IFilter Persistent Handler"},
        {"Software\\Classes\\CLSID\\" kTexFilter2Handler "\\PersistentAddinsRegistered", nullptr, ""},
        {"Software\\Classes\\CLSID"
         "\\" kTexFilter2Handler "\\PersistentAddinsRegistered\\{89BCB740-6119-101A-BCB7-00DD010655AF}",
         nullptr, kTexFilter2Clsid},
        {"Software\\Classes\\.tex\\PersistentHandler", nullptr, kTexFilter2Handler},
        {"Software\\Classes\\CLSID\\" kEpubFilter2Clsid, nullptr, "SumatraPDF IFilter"},
        {"Software\\Classes\\CLSID\\" kEpubFilter2Clsid "\\InProcServer32", nullptr, dllPath},
        {"Software\\Classes\\CLSID\\" kEpubFilter2Clsid "\\InProcServer32", "ThreadingModel", "Both"},
        {"Software\\Classes\\CLSID\\" kEpubFilter2Handler, nullptr, "SumatraPDF EPUB IFilter Persistent Handler"},
        {"Software\\Classes\\CLSID\\" kEpubFilter2Handler "\\PersistentAddinsRegistered", nullptr, ""},
        {"Software\\Classes\\CLSID"
         "\\" kEpubFilter2Handler "\\PersistentAddinsRegistered\\{89BCB740-6119-101A-BCB7-00DD010655AF}",
         nullptr, kEpubFilter2Clsid},
        {"Software\\Classes\\.epub\\PersistentHandler", nullptr, kEpubFilter2Handler},
    };
    HKEY hkey = allUsers ? HKEY_LOCAL_MACHINE : HKEY_CURRENT_USER;
    for (int i = 0; i < dimof(regVals); i++) {
        auto key = regVals[i].key;
        auto val = regVals[i].value;
        auto data = regVals[i].data;
        bool ok = LoggedWriteRegStr(hkey, key, val, data);
        if (!ok) {
            return false;
        }
    }
    return true;
}

// Note: for compat with pre-3.4 removes HKLM and HKCU keys
bool UninstallSearchFilter() {
    const char* regKeys[] = {
        // new PdfFilter2 keys
        "Software\\Classes\\CLSID\\" kPdfFilter2Clsid,
        "Software\\Classes\\CLSID\\" kPdfFilter2Handler,
        "Software\\Classes\\CLSID\\" kTexFilter2Clsid,
        "Software\\Classes\\CLSID\\" kTexFilter2Handler,
        "Software\\Classes\\CLSID\\" kEpubFilter2Clsid,
        "Software\\Classes\\CLSID\\" kEpubFilter2Handler,
        "Software\\Classes\\.pdf\\PersistentHandler",
        "Software\\Classes\\.tex\\PersistentHandler",
        "Software\\Classes\\.epub\\PersistentHandler",
        // previous PdfFilter keys
        "Software\\Classes\\CLSID\\" kPdfFilterClsid,
        "Software\\Classes\\CLSID\\" kPdfFilterHandler,
        "Software\\Classes\\CLSID\\" kTexFilterClsid,
        "Software\\Classes\\CLSID\\" kTexFilterHandler,
        "Software\\Classes\\CLSID\\" kEpubFilterClsid,
        "Software\\Classes\\CLSID\\" kEpubFilterHandler,
    };

    bool ok = true;

    for (int i = 0; i < dimof(regKeys); i++) {
        LoggedDeleteRegKey(HKEY_LOCAL_MACHINE, regKeys[i]);
        ok &= LoggedDeleteRegKey(HKEY_CURRENT_USER, regKeys[i]);
    }
    return ok;
}

bool IsSearchFilterInstalled() {
    const char* key = ".pdf\\PersistentHandler";
    char* iid = LoggedReadRegStrTemp(HKEY_CLASSES_ROOT, key, nullptr);
    bool isInstalled = str::EqI(iid, kPdfFilter2Handler) || str::EqI(iid, kPdfFilterHandler);
    logf("IsSearchFilterInstalled() isInstalled=%d\n", (int)isInstalled);
    return isInstalled;
}
