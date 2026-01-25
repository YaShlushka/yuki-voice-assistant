import os
import torch
from unsloth import FastLanguageModel
from datasets import load_dataset

max_seq_length = 512
dtype = None # None for auto detection
load_in_4bit = False
load_in_8bit=True

model, tokenizer = FastLanguageModel.from_pretrained(
    model_name = "models/llama-3.2",
    max_seq_length = max_seq_length,
    dtype = dtype,
    load_in_4bit = load_in_4bit,
    load_in_8bit = load_in_8bit
)

model = FastLanguageModel.get_peft_model(
    model,
    r=16,   # LoRA rank - suggested values: 8, 16, 32, 64, 128
    target_modules=["q_proj", "k_proj", "v_proj", "o_proj", 
                    "gate_proj", "up_proj", "down_proj"],
    lora_alpha=16,
    lora_dropout=0,   # Supports any, but = 0 is optimized
    bias="none",      # Supports any, but = "none" is optimized
    use_gradient_checkpointing="unsloth",  # Ideal for long context tuning
    random_state=3407,
    use_rslora=False,   # Disable rank-sensitive LoRA for simpler tasks
    loftq_config=None   # No LoftQ, for standard fine-tuning
)

dataset = load_dataset("json", data_files="data/train.jsonl", split="train")
dataset = dataset.select(range(4))
print(f"Using a sample size of {len(dataset)} for fine-tuning.")
