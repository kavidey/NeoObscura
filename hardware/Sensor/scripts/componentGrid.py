import csv

grid_width = 40
grid_height = 30
horizontal_distance = 70
vertical_distance = 70

component_count = 1200
letter = 'Q'  # Set a constant letter

def get_coordinates(component_number):
    row = (component_number - 1) % grid_width
    col = (component_number - 1) // grid_width
    # print(component_number, row, row - (grid_width-1) / 2, col, col - (grid_height-1) / 2)
    x = (col - (grid_height - 1) / 2) * horizontal_distance
    y = (row - (grid_width - 1) / 2) * vertical_distance
    return x, y

output_file = "component_coordinates.csv"

with open(output_file, "w", newline="") as file:
    writer = csv.writer(file, delimiter=',', quotechar='"', quoting=csv.QUOTE_ALL)
    writer.writerow(["Designator", "Footprint", "Mid X", "Mid Y", "Ref X", "Ref Y", "Pad X", "Pad Y", "Layer", "Rotation", "Comment"])  # CSV header
    writer.writerow([""])  # Last line of header as CSV header

    for i in range(1, component_count + 1):
        component_name = letter + str(i)
        x, y = get_coordinates(i)
        writer.writerow([component_name, str(x) + "mil", str(y) + "mil", str(x) + "mil", str(y) + "mil", str(x) + "mil", str(y) + "mil", "C", "0", ""])

print(f"Component coordinates saved to {output_file}.")