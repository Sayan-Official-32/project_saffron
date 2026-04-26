from fastapi import APIRouter
from app.services.ml import ml

router = APIRouter()

@router.get("/metrics")
def get_metrics():
    """
    Computes real metrics by loading datasets for all models out of ml_model/datasets
    to show accurate info on the front end. Evaluates on a test split to match the notebook's actual data.
    """
    return ml.get_metrics()
