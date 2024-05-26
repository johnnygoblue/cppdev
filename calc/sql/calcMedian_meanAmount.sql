WITH Ordered AS (
    SELECT 
        meanAmount, 
        ROW_NUMBER() OVER (ORDER BY meanAmount) AS row_num, 
        COUNT(*) OVER () AS total_count
    FROM ibfs
)
SELECT AVG(meanAmount) AS medianValue
FROM Ordered
WHERE row_num IN (
    (total_count + 1) / 2, 
    (total_count + 2) / 2
);
