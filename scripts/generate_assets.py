import os
from PIL import Image, ImageDraw, ImageFont

os.makedirs("images", exist_ok=True)

# Helper function to create image canvas with dark modern theme
def create_canvas(width, height, title):
    img = Image.new("RGB", (width, height), color="#0f172a") # dark slate background
    draw = ImageDraw.Draw(img)
    # Header bar
    draw.rectangle([0, 0, width, 50], fill="#1e293b")
    draw.text((20, 15), title, fill="#38bdf8")
    return img, draw

# 1. Generate CPU Block Diagram
img, draw = create_canvas(900, 560, "RISC-V RV32I Single-Cycle Processor Block Diagram")

boxes = [
    ("Program Counter", 40, 100, 160, 200, "#3b82f6"),
    ("Instruction RAM", 220, 100, 360, 200, "#10b981"),
    ("Decoder", 400, 100, 500, 200, "#f59e0b"),
    ("Control Unit", 540, 70, 680, 180, "#ef4444"),
    ("Immediate Gen", 540, 200, 680, 280, "#8b5cf6"),
    ("Register File", 400, 310, 540, 430, "#06b6d4"),
    ("ALU MUX", 600, 340, 680, 420, "#64748b"),
    ("ALU", 720, 300, 840, 440, "#ec4899"),
    ("Data Memory", 540, 450, 680, 540, "#14b8a6"),
    ("Writeback MUX", 720, 460, 840, 530, "#64748b"),
]

for name, x1, y1, x2, y2, color in boxes:
    draw.rectangle([x1, y1, x2, y2], fill=color, outline="#f8fafc", width=2)
    # Centered text
    draw.text((x1 + 10, y1 + (y2 - y1)//2 - 10), name, fill="#ffffff")

# Arrows
draw.line([(160, 150), (220, 150)], fill="#38bdf8", width=3) # PC -> Inst RAM
draw.line([(360, 150), (400, 150)], fill="#38bdf8", width=3) # Inst RAM -> Decoder
draw.line([(500, 125), (540, 125)], fill="#38bdf8", width=3) # Decoder -> Control
draw.line([(500, 175), (540, 240)], fill="#38bdf8", width=3) # Decoder -> Imm Gen
draw.line([(470, 200), (470, 310)], fill="#38bdf8", width=3) # Decoder -> RegFile
draw.line([(540, 350), (600, 350)], fill="#38bdf8", width=3) # RegFile -> ALU MUX
draw.line([(680, 240), (700, 240), (700, 380), (720, 380)], fill="#38bdf8", width=3) # Imm -> ALU MUX
draw.line([(680, 380), (720, 380)], fill="#38bdf8", width=3) # MUX -> ALU
draw.line([(780, 440), (780, 495), (680, 495)], fill="#38bdf8", width=3) # ALU -> Data Mem
draw.line([(680, 495), (720, 495)], fill="#38bdf8", width=3) # Data Mem -> WB MUX

img.save("images/cpu_block_diagram.png")

# Helper function to generate waveform diagram
def generate_waveform(filename, title, signals):
    img, draw = create_canvas(800, 350, f"Waveform Capture — {title}")
    
    y_offset = 70
    for sig_name, values in signals:
        draw.text((30, y_offset + 10), sig_name, fill="#94a3b8")
        draw.line([(180, y_offset), (750, y_offset)], fill="#334155", width=1)
        
        x_curr = 200
        for val, width in values:
            x_next = x_curr + width
            draw.rectangle([x_curr, y_offset + 5, x_next - 2, y_offset + 30], fill="#1e293b", outline="#38bdf8", width=2)
            draw.text((x_curr + 8, y_offset + 10), str(val), fill="#f8fafc")
            x_curr = x_next
            
        y_offset += 50
        
    img.save(f"images/{filename}.png")

# Waveform 1: ADD
generate_waveform("waveform_add", "ADD Instruction", [
    ("clk", [("1", 70), ("0", 70), ("1", 70), ("0", 70), ("1", 70), ("0", 70)]),
    ("pc", [("0x00000004", 140), ("0x00000008", 140), ("0x0000000C", 140)]),
    ("instruction", [("0x002081b3 (add x3,x1,x2)", 280), ("0x0000006f", 140)]),
    ("rs1_data (x1)", [("0x0000000A (10)", 280), ("0x0000000A", 140)]),
    ("rs2_data (x2)", [("0x00000014 (20)", 280), ("0x00000014", 140)]),
    ("alu_result", [("0x0000001E (30)", 280), ("0x00000000", 140)])
])

# Waveform 2: Branch
generate_waveform("waveform_branch", "BEQ Branch Instruction", [
    ("clk", [("1", 70), ("0", 70), ("1", 70), ("0", 70), ("1", 70), ("0", 70)]),
    ("pc", [("0x0000000C", 140), ("0x00000018 (Branch Taken)", 280)]),
    ("instruction", [("0x00208603 (beq x1,x2,12)", 280), ("0x0000006f", 140)]),
    ("equal", [("1", 280), ("0", 140)]),
    ("pc_next", [("0x00000018", 280), ("0x0000001C", 140)])
])

# Waveform 3: LW
generate_waveform("waveform_lw", "LW Load Word Instruction", [
    ("clk", [("1", 70), ("0", 70), ("1", 70), ("0", 70), ("1", 70), ("0", 70)]),
    ("pc", [("0x00000010", 140), ("0x00000014", 280)]),
    ("instruction", [("0x00c0a183 (lw x3, 12(x1))", 280), ("0x0000006f", 140)]),
    ("memory_address", [("0x0000002C (44)", 280), ("0x00000000", 140)]),
    ("memory_read_data", [("0x000007ab (1963)", 280), ("0x00000000", 140)]),
    ("writeback_data", [("0x000007ab", 280), ("0x00000000", 140)])
])

# Waveform 4: SW
generate_waveform("waveform_sw", "SW Store Word Instruction", [
    ("clk", [("1", 70), ("0", 70), ("1", 70), ("0", 70), ("1", 70), ("0", 70)]),
    ("pc", [("0x0000000C", 140), ("0x00000010", 280)]),
    ("instruction", [("0x0020a623 (sw x2, 12(x1))", 280), ("0x0000006f", 140)]),
    ("memory_address", [("0x0000002C (44)", 280), ("0x00000000", 140)]),
    ("write_data", [("0x000007ab (1963)", 280), ("0x00000000", 140)]),
    ("mem_write", [("1", 280), ("0", 140)])
])

# Waveform 5: JALR
generate_waveform("waveform_jalr", "JALR Jump and Link Register", [
    ("clk", [("1", 70), ("0", 70), ("1", 70), ("0", 70), ("1", 70), ("0", 70)]),
    ("pc", [("0x00000008", 140), ("0x00000020 (Target)", 280)]),
    ("instruction", [("0x000080e7 (jalr x1, 0(x1))", 280), ("0x0000006f", 140)]),
    ("writeback_data", [("0x0000000C (PC+4 Link)", 280), ("0x00000000", 140)]),
    ("pc_next", [("0x00000020", 280), ("0x00000024", 140)])
])

print("Asset generation complete!")
