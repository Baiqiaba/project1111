package td;

/*��Ʒ��*/
public class Product {
	private String name;
	private String type;
	private Double distance;



	public void setName(String name){
		this.name = name ;
	}
	
	public String getName(){
		return this.name;
	}
	
	public void setType(String type){
		this.type = type;
	}
	
	public  String getType(){
		return this.type;
	}
	
	public void setDistance(Double distance){
		this.distance = distance;
	}

	public Double getDistance(){
		return this.distance;
	}
}
