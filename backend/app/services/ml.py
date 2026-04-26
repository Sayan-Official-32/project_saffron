import os
import joblib
import pandas as pd
import numpy as np
from sklearn.metrics import mean_absolute_error, mean_squared_error, r2_score
from sklearn.model_selection import train_test_split
from sklearn.ensemble import GradientBoostingRegressor, RandomForestRegressor
from app.core.config import settings

class MLService:
    def __init__(self):
        self.models = {}
        self.load_models()

    def load_models(self):
        phases = ["phase1", "phase2", "phase3", "phase4"]
        for phase in phases:
            try:
                model_path = os.path.join(settings.ML_MODEL_DIR, f"{phase}_model.pkl")
                if os.path.exists(model_path):
                    self.models[phase] = joblib.load(model_path)
            except Exception as e:
                print(f"Error loading {phase} model: {e}")

    def predict_growth(self, phase: str, features: dict):
        if phase not in self.models:
            raise ValueError(f"Model for {phase} not found")
        
        features_df = pd.DataFrame([features], columns=["Temperature", "Humidity", "Light", "Soil_Moisture", "CO2"])
        model = self.models[phase]
        prediction = model.predict(features_df)[0]
        prediction = max(0.0, min(100.0, float(prediction)))
        return round(prediction, 2)

    def get_metrics(self):
        phases = ["phase1", "phase2", "phase3", "phase4"]
        metrics_data = []

        for phase in phases:
            if phase not in self.models:
                metrics_data.append({"phase": phase, "error": "Model not loaded"})
                continue
                
            csv_path = os.path.join(settings.DATASETS_DIR, f"{phase}_saffron.csv")
            try:
                df = pd.read_csv(csv_path)
                df = df.fillna(df.mean())
                X = df.drop("Growth", axis=1)
                y = df["Growth"]
                
                X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)
                model = self.models[phase]
                
                if "Gradient Boosting" in str(type(model)):
                    eval_model = GradientBoostingRegressor(random_state=42)
                    model_type = "Gradient Boosting Regressor"
                else:
                    eval_model = RandomForestRegressor(n_estimators=100, random_state=42)
                    model_type = "Random Forest Regressor"
                    
                eval_model.fit(X_train, y_train)
                y_pred = eval_model.predict(X_test)
                
                mae = mean_absolute_error(y_test, y_pred)
                rmse = np.sqrt(mean_squared_error(y_test, y_pred))
                r2 = r2_score(y_test, y_pred)
                
                metrics_data.append({
                    "phase": phase,
                    "model_type": model_type,
                    "mae": round(mae, 4),
                    "rmse": round(rmse, 4),
                    "r2": round(r2, 4)
                })
            except Exception as e:
                metrics_data.append({"phase": phase, "error": str(e)})

        return metrics_data

ml = MLService()
