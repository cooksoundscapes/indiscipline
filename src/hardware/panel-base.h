#pragma once
#include <functional>
#include <string>

class PanelBase {
public:
  using ReadCB = std::function<void(std::string, int, int)>;
  virtual void registerCallback(std::string name, ReadCB cb) = 0;
	virtual void setCurrentCallback(std::string cb) = 0;
};