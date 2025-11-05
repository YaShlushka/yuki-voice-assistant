#include "request.h"
#include "common.h"
#include "websites_links.h"

void ApplyRequest(Request request) {
	switch(request.type) {
		case RequestType::OPEN_WEB_SITE:
			if(WEBSITES_LINKS.find(request.arg) != WEBSITES_LINKS.end()) {
				OpenWebSite(request.arg);
			}
		case RequestType::OPEN_APPLICATION:
			
	}
}
