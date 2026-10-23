// language: ObjC++, file: src/TssGuard.mm, target: iOS arm64 (SMLnobita)
// Mirrors 6bohuel AWSS3 __mod_init_func 0x2eea7b0: neutralize the TssSDK lifecycle
// classes found in anogs, then drop outbound anti-cheat reports: URL/body blocklists,
// decoy request to http://127.0.0.1:1/void, fake HTTP 200 answer for blocked callbacks,
// and removal of netdata.bin from directory listings.
#import <Foundation/Foundation.h>
#import <objc/runtime.h>
#include <dispatch/dispatch.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

static void sml_stub_void(id self, SEL _cmd) { (void)self; (void)_cmd; }
static id sml_stub_id(id self, SEL _cmd) { (void)self; (void)_cmd; return nil; }
static long long sml_stub_integer(id self, SEL _cmd) { (void)self; (void)_cmd; return 0; }
static float sml_stub_float(id self, SEL _cmd) { (void)self; (void)_cmd; return 0.0f; }
static double sml_stub_double(id self, SEL _cmd) { (void)self; (void)_cmd; return 0.0; }

static IMP sml_stub_for_type(const char *encoding) {
    char type = encoding ? encoding[0] : 'v';
    switch (type) {
        case 'v':
            return (IMP)sml_stub_void;
        case 'c': case 'C': case 'B': case 's': case 'S':
        case 'i': case 'I': case 'l': case 'L':
        case 'q': case 'Q': case 'z': case 'Z':
            return (IMP)sml_stub_integer;
        case 'f':
            return (IMP)sml_stub_float;
        case 'd':
            return (IMP)sml_stub_double;
        default:
            return (IMP)sml_stub_id;
    }
}

static void sml_stub_class_methods(Class cls) {
    if (!cls) return;
    for (int pass = 0; pass < 2; pass++) {
        Class target = pass ? object_getClass(cls) : cls;
        unsigned int count = 0;
        Method *methods = class_copyMethodList(target, &count);
        if (!methods) continue;
        for (unsigned int i = 0; i < count; i++) {
            const char *encoding = method_getTypeEncoding(methods[i]);
            method_setImplementation(methods[i], sml_stub_for_type(encoding));
        }
        free(methods);
    }
}

static IMP sml_original_task;
static IMP sml_original_task_completion;
static IMP sml_original_task_url_completion;
static IMP sml_original_upload_from_data;
static IMP sml_original_sync_request;
static IMP sml_original_contents_of_directory;

// AWSS3 sub_2F0BC44: 23 URL fragments, matched against the lowercased absoluteString.
static NSString *const sml_url_blocklist[] = {
    @"anticheatexpert", @"anticheat", @"anogs", @"anogreport", @"anosdk",
    @"tdatamaster", @"tdm.qq", @"mdt.qq", @"mna.qq", @"beacon.qq",
    @"beacon-tj.qq", @"snowflake.qq", @"bugly", @"dd.garena.com",
    @"itop.kg.garena", @"bugly-aos.cros.garena", @"kg.midas.garena",
    @"oth.eve.mdt", @"tdm_report", @"corereport", @"scan_result",
    @"heartbeat_check", @"hbcheck",
};

// AWSS3 sub_2F0D824: 12 fragments matched inside the first 1024 body bytes.
static NSString *const sml_body_blocklist[] = {
    @"tdm_report", @"COREREPORT", @"HBCheck", @"black_module_macho",
    @"cheat_open_id", @"shell_report", @"ms_scan_start", @"ms_curr_mod",
    @"ts2_mod", @"ScanCast", @"AnogsReport", @"AnoSDK",
};

static BOOL sml_url_blocked(NSString *urlString) {
    if (!urlString.length) return NO;
    NSString *lower = [urlString lowercaseString];
    for (size_t i = 0; i < sizeof(sml_url_blocklist) / sizeof(sml_url_blocklist[0]); i++) {
        if ([lower containsString:sml_url_blocklist[i]]) return YES;
    }
    return NO;
}

// AWSS3 sub_2F0B908.
static BOOL sml_is_report_body(NSData *body) {
    if (![body isKindOfClass:[NSData class]]) return NO;
    const uint8_t *bytes = (const uint8_t *)body.bytes;
    NSUInteger length = body.length;
    if (length < 4) return NO;
    if (memcmp(bytes, "ano_", 4) == 0 || memcmp(bytes, "tss_", 4) == 0) return YES;
    if ((bytes[0] == 0xE5 && bytes[1] == 0xAC) || (bytes[0] == 0xAC && bytes[1] == 0xE5)) return YES;
    NSUInteger prefixLength = length > 1024 ? 1024 : length;
    NSData *prefix = [body subdataWithRange:NSMakeRange(0, prefixLength)];
    NSString *text = [[NSString alloc] initWithData:prefix encoding:NSUTF8StringEncoding];
    if (!text.length) return NO;
    for (size_t i = 0; i < sizeof(sml_body_blocklist) / sizeof(sml_body_blocklist[0]); i++) {
        if ([text containsString:sml_body_blocklist[i]]) return YES;
    }
    return NO;
}

static BOOL sml_request_blocked(NSURLRequest *request) {
    if (!request) return NO;
    if (sml_url_blocked([[request URL] absoluteString])) return YES;
    NSString *method = [request HTTPMethod];
    if (method && ([method isEqualToString:@"POST"] || [method isEqualToString:@"PUT"])) {
        if (sml_is_report_body([request HTTPBody])) return YES;
    }
    return NO;
}

static NSURLRequest *sml_decoy_request(void) {
    NSURL *decoy = [NSURL URLWithString:@"http://127.0.0.1:1/void"];
    return decoy ? [NSURLRequest requestWithURL:decoy] : nil;
}

static NSURLResponse *sml_fake_response(NSURL *url) {
    NSURL *target = url ? url : [NSURL URLWithString:@"https://report.local/"];
    return [[NSHTTPURLResponse alloc] initWithURL:target
                                       statusCode:200
                                      HTTPVersion:@"HTTP/1.1"
                                     headerFields:@{}];
}

typedef void (^SMLCompletion)(NSData *, NSURLResponse *, NSError *);

static void (^const sml_noop_completion)(NSData *, NSURLResponse *, NSError *) =
    ^(NSData *data, NSURLResponse *response, NSError *error) {
        (void)data; (void)response; (void)error;
    };

static void sml_call_completion(SMLCompletion completion, NSURL *url) {
    if (!completion) return;
    NSURLResponse *response = sml_fake_response(url);
    dispatch_async(dispatch_get_global_queue(QOS_CLASS_UTILITY, 0), ^{
        completion([NSData data], response, nil);
    });
}

static NSURLSessionDataTask *sml_data_task(id self, SEL _cmd, NSURLRequest *request) {
    if (sml_request_blocked(request)) {
        NSURLRequest *decoy = sml_decoy_request();
        if (decoy) request = decoy;
    }
    return ((NSURLSessionDataTask *(*)(id, SEL, NSURLRequest *))sml_original_task)(self, _cmd, request);
}

static NSURLSessionDataTask *sml_data_task_completion(id self, SEL _cmd, NSURLRequest *request, SMLCompletion completion) {
    if (!sml_request_blocked(request)) {
        return ((NSURLSessionDataTask *(*)(id, SEL, NSURLRequest *, SMLCompletion))sml_original_task_completion)(self, _cmd, request, completion);
    }
    sml_call_completion(completion, [request URL]);
    NSURLRequest *decoy = sml_decoy_request();
    if (!decoy) decoy = request;
    return ((NSURLSessionDataTask *(*)(id, SEL, NSURLRequest *, SMLCompletion))sml_original_task_completion)(self, _cmd, decoy, sml_noop_completion);
}

static NSURLSessionDataTask *sml_data_task_url_completion(id self, SEL _cmd, NSURL *url, SMLCompletion completion) {
    if (!sml_url_blocked([url absoluteString])) {
        return ((NSURLSessionDataTask *(*)(id, SEL, NSURL *, SMLCompletion))sml_original_task_url_completion)(self, _cmd, url, completion);
    }
    sml_call_completion(completion, url);
    NSURL *decoy = [NSURL URLWithString:@"http://127.0.0.1:1/void"];
    if (!decoy) decoy = url;
    return ((NSURLSessionDataTask *(*)(id, SEL, NSURL *, SMLCompletion))sml_original_task_url_completion)(self, _cmd, decoy, sml_noop_completion);
}

static NSURLSessionUploadTask *sml_upload_task_from_data(id self, SEL _cmd, NSURLRequest *request, NSData *body, SMLCompletion completion) {
    BOOL blocked = sml_url_blocked([[request URL] absoluteString]) || sml_is_report_body(body) || sml_is_report_body([request HTTPBody]);
    if (!blocked) {
        return ((NSURLSessionUploadTask *(*)(id, SEL, NSURLRequest *, NSData *, SMLCompletion))sml_original_upload_from_data)(self, _cmd, request, body, completion);
    }
    sml_call_completion(completion, [request URL]);
    NSURLRequest *decoy = sml_decoy_request();
    if (!decoy) decoy = request;
    return ((NSURLSessionUploadTask *(*)(id, SEL, NSURLRequest *, NSData *, SMLCompletion))sml_original_upload_from_data)(self, _cmd, decoy, [NSData data], sml_noop_completion);
}

static NSData *sml_sync_request(id self, SEL _cmd, NSURLRequest *request, NSURLResponse **response, NSError **error) {
    if (!sml_request_blocked(request)) {
        return ((NSData *(*)(id, SEL, NSURLRequest *, NSURLResponse **, NSError **))sml_original_sync_request)(self, _cmd, request, response, error);
    }
    if (response) *response = sml_fake_response([request URL]);
    if (error) *error = nil;
    return [NSData data];
}

static NSArray *sml_contents_of_directory(id self, SEL _cmd, NSString *path, NSError **error) {
    NSArray *entries = ((NSArray *(*)(id, SEL, NSString *, NSError **))sml_original_contents_of_directory)(self, _cmd, path, error);
    if (!entries) return nil;
    NSMutableArray *kept = [NSMutableArray arrayWithCapacity:entries.count];
    BOOL removed = NO;
    for (id entry in entries) {
        const char *name = [entry isKindOfClass:[NSString class]] ? [(NSString *)entry UTF8String] : NULL;
        if (name && strcasecmp(name, "netdata.bin") == 0) {
            removed = YES;
        } else {
            [kept addObject:entry];
        }
    }
    return removed ? [kept copy] : entries;
}

static void sml_hook(Class cls, const char *selector_name, IMP replacement, IMP *storage) {
    if (!cls) return;
    SEL selector = sel_registerName(selector_name);
    Method method = class_getInstanceMethod(cls, selector);
    if (!method) return;
    IMP current = method_getImplementation(method);
    if (current == replacement) return;
    if (storage && *storage == NULL) *storage = current;
    method_setImplementation(method, replacement);
}

static void sml_hook_class_method(Class cls, const char *selector_name, IMP replacement, IMP *storage) {
    if (!cls) return;
    SEL selector = sel_registerName(selector_name);
    Method method = class_getClassMethod(cls, selector);
    if (!method) return;
    IMP current = method_getImplementation(method);
    if (current == replacement) return;
    if (storage && *storage == NULL) *storage = current;
    method_setImplementation(method, replacement);
}

static BOOL sml_contains_ci(const char *haystack, const char *needle) {
    if (!haystack || !needle || !needle[0]) return NO;
    size_t length = strlen(needle);
    for (const char *p = haystack; *p; p++) {
        size_t i = 0;
        while (i < length && p[i]) {
            char a = p[i], b = needle[i];
            if (a >= 'A' && a <= 'Z') a = (char)(a - 'A' + 'a');
            if (b >= 'A' && b <= 'Z') b = (char)(b - 'A' + 'a');
            if (a != b) break;
            i++;
        }
        if (i == length) return YES;
    }
    return NO;
}

static BOOL sml_class_matches(const char *name) {
    if (!name || !name[0]) return NO;
    if (sml_contains_ci(name, "anosdk")) return YES;
    if (sml_contains_ci(name, "anticheat")) return YES;
    if (sml_contains_ci(name, "tssdk")) return YES;
    if (name[0] == 'T' && name[1] == 's' && name[2] == 's') return YES;
    return NO;
}

static void sml_stub_matching_classes(void) {
    unsigned int count = 0;
    Class *classes = objc_copyClassList(&count);
    if (!classes) return;
    for (unsigned int i = 0; i < count; i++) {
        const char *name = class_getName(classes[i]);
        if (sml_class_matches(name)) sml_stub_class_methods(classes[i]);
    }
    free(classes);
}

__attribute__((constructor)) static void sml_tss_guard_init(void) {
    static const char *classes[] = {
        "PluginTssSDKLifecycle", "TssSDKLifecycle", "TssReportManager",
        "AnoSDKCore", "AnoSDKReport", "AntiCheatReporter", "AntiCheatLifecycle"
    };
    for (size_t i = 0; i < sizeof(classes) / sizeof(classes[0]); i++) {
        sml_stub_class_methods(objc_getClass(classes[i]));
    }
    sml_stub_matching_classes();
    Class session = objc_getClass("NSURLSession");
    sml_hook(session, "dataTaskWithRequest:", (IMP)sml_data_task, &sml_original_task);
    sml_hook(session, "dataTaskWithRequest:completionHandler:", (IMP)sml_data_task_completion, &sml_original_task_completion);
    sml_hook(session, "dataTaskWithURL:completionHandler:", (IMP)sml_data_task_url_completion, &sml_original_task_url_completion);
    sml_hook(session, "uploadTaskWithRequest:fromData:completionHandler:", (IMP)sml_upload_task_from_data, &sml_original_upload_from_data);
    Class connection = objc_getClass("NSURLConnection");
    sml_hook_class_method(connection, "sendSynchronousRequest:returningResponse:error:", (IMP)sml_sync_request, &sml_original_sync_request);
    Class fileManager = objc_getClass("NSFileManager");
    sml_hook(fileManager, "contentsOfDirectoryAtPath:error:", (IMP)sml_contents_of_directory, &sml_original_contents_of_directory);
}
