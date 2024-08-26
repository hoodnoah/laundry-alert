#ifndef ERRORCODES_H
#define ERRORCODES_H

enum class ErrorCode
{
  Success = 0,
  IMUInitializationFailure,
  IMUNotInitialized,
  IMUNotReady,
  WiFiNoModule,
  WiFiFirmwareOutdated,
  WiFiClientConnectFailed,
  WiFiClientTimeout,
  HttpClientPostFailed,
  HttpClientPostRejected,
};

#endif