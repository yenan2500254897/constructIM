void
ircd::init(boost::asio::executor &&executor)
try
{

	// Setup the core event loop system starting with the user's supplied ios.
	ios::init(std::move(executor));

	context main_context
	{
		"main",
		512_KiB,
		std::bind(&ircd::printStr, std::move("main context")),
		context::POST | context::SLICE_EXEMPT
	};
    main_context.detach();
    ctx::ole::init _ole_; 
    context user_context
	{
		"user",
		512_KiB,
		std::bind(&ircd::printStr, std::move("user context")),
		context::POST | context::SLICE_EXEMPT
	};
    user_context.detach();

	// The default behavior for ircd::context is to join the ctx on dtor. We
	// can't have that here because this is strictly an asynchronous function
	// on the main stack. Under normal circumstances, the mc will be entered
	// and be able to delete this pointer itself when it finishes. Otherwise
	// this must be manually deleted with assurance that mc will never enter.
	
}
catch(const std::exception &e)
{
	throw;
}


void
ircd::printStr(std::string s) {
    std::cout<<"hello "<<s<<std::endl;
}