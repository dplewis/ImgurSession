//
//  IMGClient.h
//  ImgurSession
//
//  Created by Johann Pardanaud on 29/06/13.
//  Distributed under the MIT license.
//


#import "IMGAccountRequest.h"
#import "IMGAlbumRequest.h"
#import "IMGCommentRequest.h"
#import "IMGConversationRequest.h"
#import "IMGGalleryRequest.h"
#import "IMGImageRequest.h"
#import "IMGNotificationRequest.h"

#import "IMGModel.h"

static NSString * const IMGBaseURL = @"https://api.imgur.com";
static NSString * const IMGAPIVersion = @"3";
static NSString * const IMGOAuthEndpoint = @"oauth2/token";

static NSString * const IMGHeaderUserLimit = @"X-RateLimit-UserLimit";
static NSString * const IMGHeaderUserRemaining = @"X-RateLimit-UserRemaining";
static NSString * const IMGHeaderUserReset = @"X-RateLimit-UserReset";
static NSString * const IMGHeaderClientLimit = @"X-RateLimit-ClientLimit";
static NSString * const IMGHeaderClientRemaining = @"X-RateLimit-ClientRemaining";

//notifications
static NSString * const IMGRateLimitExceededNotification = @"IMGRateLimitExceededNotification";
static NSString * const IMGRateLimitNearLimitNotification = @"IMGRateLimitNearLimitNotification";
static NSString * const IMGNeedsExternalWebviewNotification = @"IMGNeedsExternalWebviewNotification";
static NSString * const IMGModelFetchedNotification = @"IMGModelFetchedNotification";
static NSString * const IMGAuthChangedNotification = @"IMGAuthChangedNotification";
static NSString * const IMGAuthRefreshedNotification = @"IMGAuthRefreshedNotification";

/**
 Type of authorization to use, you will proably use PIN
 */
typedef NS_ENUM(NSInteger, IMGAuthType){
    IMGNoAuthType,
    IMGPinAuth,
    IMGTokenAuth,
    IMGCodeAuth
};

typedef NS_ENUM(NSInteger, IMGAuthState){
    IMGAuthStateNone,
    IMGAuthStateAuthenticated,
    IMGAuthStateExpired
};


/**
 Protocol to be alerted of ImgurSession notifcations. Called on main thread.
 */
@protocol IMGSessionDelegate <NSObject>

@required

/**
 Alerts delegate that request limit is hit
 */
-(void)imgurSessionRateLimitExceeded;
/**
 Alerts delegate that webview is needed to present Imgur OAuth authentication
 */
-(void)imgurSessionNeedsExternalWebview:(NSURL*)url;

@optional

/**
 Alerts delegate that request limit is being approached
 */
-(void)imgurSessionNearRateLimit:(NSInteger)remainingRequests;
/**
 Informs delegate of new model objects
 */
-(void)imgurSessionModelFetched:(id)model;
/**
 Informs delegate of new token refreshs
 */
-(void)imgurSessionTokenRefreshed;
/**
 Informs delegate of new authentication success
 */
-(void)imgurSessionAuthStateChanged:(IMGAuthState)state;

@end


/**
 Session manager class for ImgurSession Session instance. Controls all requests by subclassing AFHTTPSessionManager
 */
@interface IMGSession : AFHTTPSessionManager


// client authorization

/**
 App Id as registered with Imgur at http://imgur.com/account/settings/apps
 */
@property (readonly, nonatomic,copy) NSString *clientID;
/**
 App Secret as registered with Imgur at http://imgur.com/account/settings/apps
 */
@property (readonly, nonatomic, copy) NSString *secret;
/**
 Refresh token as retrieved from oauth/token GET request with PIN
 */
@property (readonly, nonatomic, copy) NSString *refreshToken;
/**
 Access token as retrieved from oauth/token GET request with PIN. Expires after 1 hour after retrieval
 */
@property (readonly, nonatomic, copy) NSString *accessToken;
/**
 Access token expiry date
 */
@property (readonly, nonatomic) NSDate *accessTokenExpiry;
/**
 Access token expiry date
 */
@property (readonly, nonatomic) IMGAuthType lastAuthType;


// rate limiting

/**
 Requests current user has remaining
 */
@property (readonly,nonatomic) NSInteger creditsUserRemaining;
/**
 Daily limit on user requests
 */
@property (readonly,nonatomic) NSInteger creditsUserLimit;
/**
 unix epoch date when user credits are reset
 */
@property (readonly,nonatomic) NSInteger creditsUserReset;
/**
 Requests app has remaining
 */
@property (readonly,nonatomic) NSInteger creditsClientRemaining;
/**
 Daily limit for the app
 */
@property (readonly,nonatomic) NSInteger creditsClientLimit;

/**
 Warn client after going below this number of available requests. The default is 100 requests.
 */
@property  (readonly,nonatomic) NSInteger warnRateLimit;

/**
 Required delegate to warn of imgur events.
 */
@property (weak) id<IMGSessionDelegate> delegate;


#pragma mark - Initialize

/**
 @return Session manager
 */
+ (instancetype)sharedInstance;
/**
 @param clientID    client Id string as registered with Imgur
 @param secret    secret string as registered with Imgur
 @return            Session manager for interacting with Imgur account
 */
+ (instancetype)sharedInstanceWithClientID:(NSString *)clientID secret:(NSString *)secret;
/**
 @param clientID    client Id string as registered with Imgur
 @param secret    secret string as registered with Imgur
 @return            Session manager
 */
- (instancetype)initWithClientID:(NSString *)clientID secret:(NSString *)secret;


#pragma mark - Authentication

/**
 Retrieves URL associated with website authorization page
 @param authType     authorization type pin,code,token
 @return    authorization URL to open in Webview or Safari
 */
-(void)refreshAuthentication:(void (^)(NSString *))success failure:(void (^)(NSError *error))failure;
/**
 Retrieves URL associated with website authorization page
 @param authType     authorization type pin,code,token
 @return    authorization URL to open in Webview or Safari
 */
- (NSURL *)authenticateWithLink;
/**
 Retrieves URL associated with website authorization page
 @param authType     authorization type pin,code,token
 @return    authorization URL to open in Webview or Safari
 */
- (NSURL *)authenticateWithExternalURLForType:(IMGAuthType)authType;
/**
 Requests access tokens using inputted pin code
 @param authType     authorization type pin,code,token
 @param code     code input string for authorization
 @param success     success completion
 @param failure     failure completion
 */
- (void)authenticateWithType:(IMGAuthType)authType withCode:(NSString*)code success:(void (^)(NSString * accessToken))success failure:(void (^)(NSError *error))failure;


#pragma mark - Requests

-(NSURLSessionDataTask *)PUT:(NSString *)URLString parameters:(NSDictionary *)parameters success:(void (^)(NSURLSessionDataTask *, id))success failure:(void (^)( NSError *))failure;

-(NSURLSessionDataTask *)DELETE:(NSString *)URLString parameters:(NSDictionary *)parameters success:(void (^)(NSURLSessionDataTask *, id))success failure:(void (^)( NSError *))failure;

-(NSURLSessionDataTask *)POST:(NSString *)URLString parameters:(NSDictionary *)parameters success:(void (^)(NSURLSessionDataTask *, id))success failure:(void (^)( NSError *))failure;

-(NSURLSessionDataTask *)GET:(NSString *)URLString parameters:(NSDictionary *)parameters success:(void (^)(NSURLSessionDataTask *, id))success failure:(void (^)(NSError *))failure;

-(NSURLSessionDataTask *)POST:(NSString *)URLString parameters:(NSDictionary *)parameters constructingBodyWithBlock:(void (^)(id<AFMultipartFormData>))block success:(void (^)(NSURLSessionDataTask *, id))success failure:(void (^)(NSError *))failure;


#pragma mark - Rate Limit Tracking

/**
 Updates session rate limit tracking with the headers of an AFNetworking Response
 @param response     the HTTP response
 */
-(void)updateClientRateLimiting:(NSHTTPURLResponse*)response;

#pragma mark - Model Tracking

/**
 Alerts session to a new model fetch
 @param model     the model fetched
 */
-(void)trackModelObjectsForDelegateHandling:(id)model;




@end
