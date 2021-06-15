#include <iostream>

#define OLC_PGE_APPLICATION
#include "pge/olcPixelGameEngine.h"

#include "DebugLogger.h"

class PGEApplication : public olc::PixelGameEngine
{
public:
	PGEApplication()
	{
		LOG_INFO() << "PGEApplication created";
		sAppName = "PGEApplication - SpriteBatch demo";
	}

	virtual ~PGEApplication() {
		LOG_INFO() << "PGEApplication destroyed";
	}

public:
	bool OnUserCreate() override
	{
		LOG_INFO() << "PGEApplication::OnUserCreate() initializing";
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		return true;
	}

	/**
	 * If OnUserDestroy() returns false, the application will not exit,
	 * it keeps running.
	 */
	bool OnUserDestroy() override
	{
		LOG_INFO() << "PGEApplication::OnUserDestroy()";
		return true;
	}

private:
};

int main()
{
	PGEApplication demo;
	if(demo.Construct(1024, 768, 1, 1))
		demo.Start();
	return 0;
}
