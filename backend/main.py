import os
from fastapi import FastAPI
from fastapi.staticfiles import StaticFiles
from fastapi.middleware.cors import CORSMiddleware
from app.api.router import api_router
from app.core.config import settings

def create_app() -> FastAPI:
    app = FastAPI(title="Project Saffron API", version="1.0.0")

    # Allow CORS for frontend communication
    app.add_middleware(
        CORSMiddleware,
        allow_origins=["*"], 
        allow_credentials=True,
        allow_methods=["*"],
        allow_headers=["*"],
    )

    # Include API router
    app.include_router(api_router, prefix="/api")

    # Mount static frontend
    if os.path.exists(settings.FRONTEND_DIR):
        app.mount("/", StaticFiles(directory=settings.FRONTEND_DIR, html=True), name="frontend")
    else:
        print(f"Warning: Frontend directory not found at {settings.FRONTEND_DIR}")

    return app

app = create_app()

if __name__ == "__main__":
    import uvicorn
    uvicorn.run("main:app", host="127.0.0.1", port=8001, reload=True)
