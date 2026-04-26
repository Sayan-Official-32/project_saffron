from fastapi import APIRouter
from app.api.routes import sensors, actuators, predict, metrics, history

api_router = APIRouter()

api_router.include_router(sensors.router, tags=["sensors"])
api_router.include_router(actuators.router, tags=["actuators"])
api_router.include_router(predict.router, tags=["predict"])
api_router.include_router(metrics.router, tags=["metrics"])
api_router.include_router(history.router, tags=["history"])
