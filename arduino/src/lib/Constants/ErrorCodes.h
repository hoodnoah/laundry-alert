#ifndef ERRORCODES_H
#define ERRORCODES_H

enum class ErrorCode
{
  Success = 0,
  IMUInitializationFailure,
  IMUNotReady,
  WiFiNoModule,
  WiFiFirmwareOutdated,
  WiFiClientConnectFailed,
  WiFiClientTimeout,
  HttpClientPostFailed,
  HttpClientPostRejected,
};

#endif