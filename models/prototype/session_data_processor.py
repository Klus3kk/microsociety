import json
import pandas as pd
import numpy as np
from pathlib import Path
import argparse
from datetime import datetime
import os

class SessionDataProcessor:
    def __init__(self, session_dir="training_data/sessions"):
        self.session_dir = Path(session_dir)
        self.all_experiences = []
        
    def process_all_sessions(self):
        """Process all session files and merge them"""
        print(f"Processing session files from: {self.session_dir}")
        
        if not self.session_dir.exists():
            print(f"Session directory {self.session_dir} does not exist!")
            return False
            
        json_files = list(self.session_dir.glob("*.json"))
        print(f"Found {len(json_files)} session files")
        
        total_experiences = 0
        
        for json_file in sorted(json_files):
            try:
                with open(json_file, 'r') as f:
                    data = json.load(f)
                
                experiences = data.get('experiences', [])
                if experiences:
                    self.all_experiences.extend(experiences)
                    total_experiences += len(experiences)
                    print(f"✓ {json_file.name}: {len(experiences)} experiences")
                else:
                    print(f"✗ {json_file.name}: No experiences found")
                    
            except Exception as e:
                print(f"✗ Error reading {json_file.name}: {e}")
        
        print(f"\nTotal experiences collected: {total_experiences}")
        return total_experiences > 0
    
    def analyze_data_quality(self):
        """Analyze the quality and distribution of collected data"""
        if not self.all_experiences:
            print("No data to analyze")
            return
            
        print("\n=== Data Quality Analysis ===")
        
        # Action distribution
        actions = [exp['action'] for exp in self.all_experiences]
        unique_actions, counts = np.unique(actions, return_counts=True)
        
        print("Action Distribution:")
        action_names = {
            1: "Move", 2: "ChopTree", 3: "MineRock", 4: "GatherBush",
            5: "StoreItem", 6: "UpgradeHouse", 7: "RegenerateEnergy",
            8: "TakeOutItems", 9: "BuyItem", 10: "SellItem", 11: "Rest"
        }
        
        for action, count in zip(unique_actions, counts):
            percentage = (count / len(actions)) * 100
            name = action_names.get(action, f"Action{action}")
            print(f"  {name:15}: {count:6} ({percentage:5.1f}%)")
        
        # Reward distribution
        rewards = [exp['reward'] for exp in self.all_experiences]
        print(f"\nReward Statistics:")
        print(f"  Count: {len(rewards)}")
        print(f"  Mean:  {np.mean(rewards):8.2f}")
        print(f"  Std:   {np.std(rewards):8.2f}")
        print(f"  Min:   {np.min(rewards):8.2f}")
        print(f"  Max:   {np.max(rewards):8.2f}")
        
        # NPC distribution
        npcs = [exp['npcName'] for exp in self.all_experiences]
        unique_npcs, npc_counts = np.unique(npcs, return_counts=True)
        print(f"\nNPC Contribution:")
        for npc, count in zip(unique_npcs, npc_counts):
            percentage = (count / len(npcs)) * 100
            print(f"  {npc:12}: {count:6} ({percentage:5.1f}%)")
        
        # Terminal states
        terminal_states = sum(1 for exp in self.all_experiences if exp['done'])
        print(f"\nTerminal States: {terminal_states} ({terminal_states/len(self.all_experiences)*100:.1f}%)")
    
    def save_merged_data(self, output_file="merged_training_data"):
        """Save merged data in multiple formats"""
        if not self.all_experiences:
            print("No data to save")
            return False
        
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        
        # Save as JSON (for Python training script)
        json_file = f"{output_file}_{timestamp}.json"
        merged_data = {
            "metadata": {
                "total_experiences": len(self.all_experiences),
                "processing_timestamp": timestamp,
                "source_directory": str(self.session_dir)
            },
            "data": self.all_experiences
        }
        
        with open(json_file, 'w') as f:
            json.dump(merged_data, f, indent=2)
        print(f"Saved JSON: {json_file}")
        
        # Save as CSV (for easier analysis)
        csv_file = f"{output_file}_{timestamp}.csv"
        self.save_as_csv(csv_file)
        print(f"Saved CSV: {csv_file}")
        
        # Create a symlink to latest
        try:
            if os.path.exists("latest_training_data.json"):
                os.remove("latest_training_data.json")
            if os.path.exists("latest_training_data.csv"):
                os.remove("latest_training_data.csv")
                
            os.symlink(json_file, "latest_training_data.json")
            os.symlink(csv_file, "latest_training_data.csv")
            print("Created latest_training_data symlinks")
        except:
            # Fallback for Windows
            import shutil
            shutil.copy2(json_file, "latest_training_data.json")
            shutil.copy2(csv_file, "latest_training_data.csv")
            print("Created latest_training_data copies")
        
        return True
    
    def save_as_csv(self, filename):
        """Convert to CSV format for training"""
        rows = []
        
        for exp in self.all_experiences:
            row = [
                exp['state']['posX'], exp['state']['posY'],
                exp['state']['nearbyTrees'], exp['state']['nearbyRocks'],
                exp['state']['nearbyBushes'], exp['state']['energyLevel'],
                exp['state']['inventoryLevel'], exp['action'], exp['reward'],
                exp['nextState']['posX'], exp['nextState']['posY'],
                exp['nextState']['nearbyTrees'], exp['nextState']['nearbyRocks'],
                exp['nextState']['nearbyBushes'], exp['nextState']['energyLevel'],
                exp['nextState']['inventoryLevel'], int(exp['done']),
                exp['npcName'], exp['timestamp']
            ]
            rows.append(row)
        
        df = pd.DataFrame(rows, columns=[
            'state_posX', 'state_posY', 'state_nearbyTrees', 'state_nearbyRocks',
            'state_nearbyBushes', 'state_energyLevel', 'state_inventoryLevel',
            'action', 'reward', 'nextState_posX', 'nextState_posY',
            'nextState_nearbyTrees', 'nextState_nearbyRocks', 'nextState_nearbyBushes',
            'nextState_energyLevel', 'nextState_inventoryLevel', 'done',
            'npcName', 'timestamp'
        ])
        
        df.to_csv(filename, index=False)
        return True
    
    def filter_quality_data(self, min_reward=-50, max_reward=50):
        """Filter out outlier data points"""
        original_count = len(self.all_experiences)
        
        self.all_experiences = [
            exp for exp in self.all_experiences
            if min_reward <= exp['reward'] <= max_reward
        ]
        
        filtered_count = len(self.all_experiences)
        print(f"Filtered data: {original_count} -> {filtered_count} experiences")
        print(f"Removed {original_count - filtered_count} outliers")

def main():
    parser = argparse.ArgumentParser(description='Process MicroSociety session data')
    parser.add_argument('--session-dir', default='training_data/sessions',
                       help='Directory containing session files')
    parser.add_argument('--output', default='merged_training_data',
                       help='Output filename prefix')
    parser.add_argument('--analyze-only', action='store_true',
                       help='Only analyze data, do not save')
    parser.add_argument('--filter-outliers', action='store_true',
                       help='Filter out extreme reward values')
    
    args = parser.parse_args()
    
    processor = SessionDataProcessor(args.session_dir)
    
    print("=== MicroSociety Session Data Processor ===")
    
    if not processor.process_all_sessions():
        print("No valid session data found!")
        return 1
    
    if args.filter_outliers:
        processor.filter_quality_data()
    
    processor.analyze_data_quality()
    
    if not args.analyze_only:
        if processor.save_merged_data(args.output):
            print(f"\nData processing complete!")
        else:
            print("Failed to save merged data")
            return 1
    return 0

if __name__ == "__main__":
    exit(main())